# include <SFML/Graphics.hpp>
# include <vector>
# include <unistd.h>
# include <fstream>

using namespace std;

template < typename _type >
class MATRIX
{
	private:
	unsigned int rows_no;
	unsigned int columns_no;
	_type * * matrix;

	public:
	MATRIX ()
	{
		rows_no = 0;
		columns_no = 0;
		matrix = nullptr;
	}
	void Create ( unsigned int _rows_no, unsigned int _columns_no )
	{
		if ( matrix != nullptr )
			return;
		rows_no = _rows_no;
		columns_no = _columns_no;
		matrix = new _type * [ rows_no ];
		for ( unsigned int y = 0; y < rows_no; y++ )
			matrix [ y ] = new _type [ columns_no ];
	}
	unsigned int GetRowsNo ( )
	{
		return rows_no;
	}
	unsigned int GetColumnsNo ( )
	{
		return columns_no;
	}
	void Destroy ( )
	{
		if ( matrix == nullptr )
			return;
		for ( unsigned int y = 0; y < rows_no; y++ )
			delete [ ] matrix [ y ];
		delete [ ] matrix;
		rows_no = 0;
		columns_no = 0;
	}
	void operator = ( _type * _matrix )
	{
		for ( unsigned int y = 0; y < rows_no; y++ )
			for ( unsigned int x = 0; x < columns_no; x++ )
				matrix [ y ] [ x ] = _matrix [ y * columns_no + x ];
	}
	_type * & operator [] ( unsigned int _row )
	{
		if ( _row >= rows_no )
			exit ( 1 );
		return matrix [ _row ];
	}
};

struct RGB_COLOR
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	RGB_COLOR ()
	{
		red = 0;
		green = 0;
		blue = 0;
	}
	RGB_COLOR ( unsigned char _red, unsigned char _green, unsigned char _blue )
	{
		red = _red;
		green = _green;
		blue = _blue;
	}
};

struct LAYER_INFO
{
	RGB_COLOR off;
	RGB_COLOR on;
};

class DIGITAL_IMAGE
{
	private:
	unsigned short layers_no;
	LAYER_INFO * layer_info;
	unsigned int width;
	unsigned int height;
	MATRIX < unsigned short > * matrix;

	public:
	DIGITAL_IMAGE ()
	{
		layers_no = 0;
		layer_info = nullptr;
		width = 0;
		height = 0;
		matrix = nullptr;
	}
	void Create ( unsigned short _layers_no )
	{
		if ( layers_no != 0 || _layers_no == 0 )
			return;
		layers_no = _layers_no;
		layer_info = new LAYER_INFO [ layers_no ];
	}
	void DefineLayer ( unsigned int _id, RGB_COLOR _off, RGB_COLOR _on )
	{
		if ( layers_no == 0 )
			return;
		unsigned int num_id = 0;
		while ( _id != 0 )
		{
			_id = _id >> 1;
			num_id++;
		}
		num_id--;
		if ( num_id >= layers_no )
			return;
		layer_info [ num_id ].off = _off;
		layer_info [ num_id ].on = _on;
	}
	void SetMatrix ( MATRIX < unsigned short > * _matrix )
	{
		if ( layers_no == 0 )
			return;
		if ( _matrix == nullptr )
			return;
		matrix = _matrix;
		height = matrix->GetRowsNo ();
		width = matrix->GetColumnsNo ();
	}
	void Draw ( sf::RenderWindow & _window, unsigned int _pos_x,
				unsigned int _pos_y, unsigned short _scale,
				unsigned int _settings )
	{
		if ( layers_no == 0 )
			return;
		unsigned long array_size = height * _scale * width * _scale;
		sf::Vertex * pixel_matrix = new sf::Vertex [ array_size ];
		for ( unsigned short layer_id = 0; layer_id < layers_no; layer_id++ )
		{
			unsigned int id = 1;
			for ( unsigned short i = 0; i < layer_id; i++ )
				id = id << 1;
			LAYER_INFO & layer = layer_info [ layer_id ];
			if ( _settings & id )
			{
				for ( unsigned int y = 0; y < height; y++ )
				for ( unsigned int x = 0; x < width; x++ )
					if ( ( * matrix ) [ y ] [ x ] == layer_id )
						for ( unsigned int dy = 0; dy < _scale; dy++ )
						for ( unsigned int dx = 0; dx < _scale; dx++ )
							pixel_matrix [ ( y * _scale + dy ) * ( width
								* _scale ) + x * _scale + dx ] = sf::Vertex
								( sf::Vector2f ( _pos_x + x * _scale + dx,
												 _pos_y + y * _scale + dy ),
								  sf::Color ( layer.on.red, layer.on.green,
										layer.on.blue ) );
			}
			else
			{
				for ( unsigned int y = 0; y < height; y++ )
				for ( unsigned int x = 0; x < width; x++ )
					if ( ( * matrix ) [ y ] [ x ] == layer_id )
						for ( unsigned int dy = 0; dy < _scale; dy++ )
						for ( unsigned int dx = 0; dx < _scale; dx++ )
							pixel_matrix [ ( y * _scale + dy ) * ( width
							  * _scale ) + x * _scale + dx ] = sf::Vertex
								( sf::Vector2f ( _pos_x + x * _scale + dx,
									 _pos_y + y * _scale + dy ),
								  sf::Color ( layer.off.red, layer.off.green,
									layer.off.blue ) );
			}
		}
		_window.draw ( pixel_matrix, array_size, sf::Points );
		delete [ ] pixel_matrix;
	}
	unsigned int GetWidth ( )
	{
		return width;
	}
	unsigned int GetHeight ( )
	{
		return height;
	}
	void Destroy ( )
	{
		if ( layers_no == 0 )
			return;
		matrix = nullptr;
		width = 0;
		height = 0;
		delete [ ] layer_info;
		layers_no = 0;
	}
};

struct VARIANT_DEFINITION
{
	char character;
	unsigned int style;
	VARIANT_DEFINITION ()
	{
		character = 0;
		style = 0;
	}
	VARIANT_DEFINITION ( char _character, unsigned int _style )
	{
		character = _character;
		style = _style;
	}
};

struct IMAGE_TYPE_DEFINITION
{
	vector < VARIANT_DEFINITION > variants_definitions;
	IMAGE_TYPE_DEFINITION ()
	{
	}
};

struct DISPLAY_IMAGE_INFO
{
	unsigned short image_type;
	DIGITAL_IMAGE * digital_image;
	DISPLAY_IMAGE_INFO ()
	{
		image_type = 0;
		digital_image = nullptr;
	}
	DISPLAY_IMAGE_INFO ( unsigned short _image_type, DIGITAL_IMAGE *
		_digital_image)
	{
		image_type = _image_type;
		digital_image = _digital_image;
	}
};

class DIGITAL_DISPLAY
{
	private:
	unsigned short types_no;
	IMAGE_TYPE_DEFINITION * types_definitions;
	vector < DISPLAY_IMAGE_INFO > display_definition;
	string display_string;

	public:
	DIGITAL_DISPLAY ()
	{
		types_no = 0;
		types_definitions = nullptr;
	}
	void Create ( unsigned short _types_no )
	{
		if ( types_no != 0 || _types_no == 0 )
			return;
		types_no = _types_no;
		types_definitions = new IMAGE_TYPE_DEFINITION [ types_no ];
		display_string.clear ();
	}
	void AddTypeDefinition ( unsigned short _type, char _character,
							 unsigned int _style )
	{
		if ( types_no == 0 || _type >= types_no )
			return;
		IMAGE_TYPE_DEFINITION & def = types_definitions [ _type ];
		for ( auto & variant : def.variants_definitions )
			if ( variant.character == _character )
				return;
		VARIANT_DEFINITION temp ( _character, _style );
		def.variants_definitions.push_back ( temp );
	}
	void AddImage ( unsigned short _type, DIGITAL_IMAGE * _image )
	{
		if ( _type >= types_no )
			return;
		DISPLAY_IMAGE_INFO temp ( _type, _image );
		display_definition.push_back ( temp );
	}
	void SetString ( string _new_string )
	{
		display_string = _new_string;
	}
	void DrawDisplay ( sf::RenderWindow & _window, unsigned int _pos_x,
					   unsigned int _pos_y, unsigned short _scale )
	{
		unsigned short string_pos = 0;
		unsigned int x_shift = 0;
		for ( auto & image : display_definition )
		{
			unsigned int style = 0;
			if ( string_pos < display_string.length () )
			{
				IMAGE_TYPE_DEFINITION & type
					= types_definitions [ image.image_type ];
				for ( auto & variant : type.variants_definitions )
					if ( variant.character == display_string [ string_pos ] )
					{
						style = variant.style;
						break;
					}
			}
			image.digital_image->Draw ( _window, _pos_x + x_shift,
										_pos_y, _scale, style );
			x_shift += image.digital_image->GetWidth () * _scale;
			string_pos++;
		}
	}
	void Destroy ()
	{
		if ( types_no == 0 )
			return;
		display_string.clear ();
		display_definition.clear ();
		for ( unsigned short i = 0; i < types_no; i++ )
			types_definitions [ i ].variants_definitions.clear ();
		delete [ ] types_definitions;
		types_no = 0;
	}
};

struct THREAD_ARGUMENT
{
	bool stop_signal;
	vector < void * > structures;
	vector < void * > classes;
	vector < DIGITAL_DISPLAY * > digital_displays;
	THREAD_ARGUMENT ()
	{
		stop_signal = false;
	}
};

unsigned short space_def [ ] =
	{
		0, 0, 0, 0, // --------
		0, 0, 0, 0, // --------
		0, 0, 0, 0, // --------
		0, 0, 0, 0, // --------
		0, 0, 0, 0, // --------
		0, 0, 0, 0, // --------
		0, 0, 0, 0, // --------
		0, 0, 0, 0  // --------
	};
unsigned short digit_def [ ] =
	{
		0, 1, 1, 0, 0, // --0101----
		2, 0, 0, 3, 0, // 02----03--
		2, 0, 0, 3, 0, // 02----03--
		0, 4, 4, 0, 0, // --0404----
		5, 0, 0, 6, 0, // 05----06--
		5, 0, 0, 6, 0, // 05----06--
		0, 7, 7, 0, 0, // --0707----
		0, 0, 0, 0, 0  // ----------
	};
unsigned short colon_def [ ] =
	{
		0, 0, // ----
		1, 0, // 01--
		0, 0, // ----
		0, 0, // ----
		0, 0, // ----
		1, 0, // 01--
		0, 0, // ----
		0, 0  // ----
	};
unsigned short clock_icon_def [ ] =
	{
		0, 0, 1, 1, 1, 0, 0, 0, // ----010101------
		0, 1, 0, 0, 0, 1, 0, 0, // --01------01----
		1, 0, 0, 1, 0, 0, 1, 0, // 01----01----01--
		1, 0, 0, 1, 1, 0, 1, 0, // 01----0101--01--
		1, 0, 0, 0, 0, 0, 1, 0, // 01----------01--
		0, 1, 0, 0, 0, 1, 0, 0, // --01------01----
		0, 0, 1, 1, 1, 0, 0, 0, // ----010101------
		0, 0, 0, 0, 0, 0, 0, 0  // ----------------
	};
unsigned short dot_def [ ] =
	{
		0, 0, // ----
		0, 0, // ----
		0, 0, // ----
		0, 0, // ----
		0, 0, // ----
		0, 0, // ----
		1, 0, // 01--
		0, 0  // ----
	};
unsigned short date_icon_def [ ] =
	{
		1, 1, 1, 1, 1, 1, 1, 0, // 01010101010101--
		0, 0, 0, 0, 0, 0, 0, 0, // ----------------
		1, 0, 1, 0, 1, 0, 1, 0, // 01--01--01--01--
		0, 0, 0, 0, 0, 0, 0, 0, // ----------------
		1, 0, 1, 0, 1, 0, 1, 0, // 01--01--01--01--
		0, 0, 0, 0, 0, 0, 0, 0, // ----------------
		1, 0, 1, 0, 1, 0, 0, 0, // 01--01--01------
		0, 0, 0, 0, 0, 0, 0, 0  // ----------------
	};
unsigned short system_time_icon_def [ ] =
	{
		1, 1, 1, 1, 1, 1, 1, 0, // 01010101010101--
		1, 0, 0, 0, 0, 0, 1, 0, // 01----------01--
		1, 0, 2, 0, 3, 0, 1, 0, // 01--02--03--01--
		1, 1, 2, 0, 3, 1, 1, 0, // 010102--030101--
		1, 0, 2, 0, 3, 0, 1, 0, // 01--02--03--01--
		1, 0, 0, 0, 0, 0, 1, 0, // 01----------01--
		1, 1, 1, 1, 1, 1, 1, 0, // 01010101010101--
		0, 0, 0, 0, 0, 0, 0, 0  // ----------------
	};
unsigned short proc_usage_def [ ] =
	{
		1, 0, // 01--
		1, 0, // 01--
		0, 0, // ----
		2, 0, // 02--
		2, 0, // 02--
		0, 0, // ----
		3, 0, // 03--
		3, 0  // 03--
	};
unsigned short temp_alarm_def [ ] =
	{
		0, 1, 1, 0, 1, 0, // --0101--01--
		0, 1, 0, 0, 1, 0, // --01----01--
		0, 1, 1, 0, 0, 0, // --0101------
		0, 1, 0, 0, 1, 0, // --01----01--
		0, 1, 1, 0, 0, 0, // --0101------
		1, 1, 1, 1, 0, 0, // 01010101----
		1, 1, 1, 1, 0, 0, // 01010101----
		0, 1, 1, 0, 0, 0  // --0101------
	};
unsigned short proc_icon_def [ ] =
	{
		0, 1, 0, 1, 0, 1, 0, 0, // --01--01--01----
		1, 1, 1, 1, 1, 1, 1, 0, // 01010101010101--
		0, 1, 2, 2, 2, 1, 0, 0, // --0102020201----
		1, 1, 2, 2, 2, 1, 1, 0, // 01010202020101--
		0, 1, 2, 2, 2, 1, 0, 0, // --0102020201----
		1, 1, 1, 1, 1, 1, 1, 0, // 01010101010101--
		0, 1, 0, 1, 0, 1, 0, 0, // --01--01--01----
		0, 0, 0, 0, 0, 0, 0, 0  // ----------------
	};
unsigned short fan_speed_def [ ] =
	{
		11, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 10, 0,
		11, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 8, 0, 9, 0, 10, 0,
		11, 0, 11, 0, 0, 0, 0, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0,
		0, 0, 0, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0,
		1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0,
		1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0,
		1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0,
		1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 10, 0
		// 11--11--------------------------09--10--
		// 11--11------------------07--08--09--10--
		// 11--11----------05--06--07--08--09--10--
		// --------03--04--05--06--07--08--09--10--
		// 01--02--03--04--05--06--07--08--09--10--
		// 01--02--03--04--05--06--07--08--09--10--
		// 01--02--03--04--05--06--07--08--09--10--
		// 01--02--03--04--05--06--07--08--09--10--
	};
unsigned short fan_speed_delta_def [ ] =
	{
		0, 1, 0, 0, // --01----
		1, 1, 1, 0, // 010101--
		0, 0, 0, 0, // --------
		0, 0, 0, 0, // --------
		0, 0, 0, 0, // --------
		2, 2, 2, 0, // 020202--
		0, 2, 0, 0, // --02----
		0, 0, 0, 0  // --------
	};
unsigned short fan_icon_def [ ] =
	{
		0, 1, 1, 1, 0, 0, 0, 0, // --010101--------
		0, 0, 1, 1, 0, 0, 1, 0, // ----0101----01--
		0, 0, 0, 1, 0, 1, 1, 0, // ------01--0101--
		1, 1, 1, 1, 1, 1, 1, 0, // 01010101010101--
		1, 1, 0, 1, 0, 0, 0, 0, // 0101--01--------
		1, 0, 0, 1, 1, 0, 0, 0, // 01----0101------
		0, 0, 0, 1, 1, 1, 0, 0, // ------010101----
		0, 0, 0, 0, 0, 0, 0, 0  // ----------------
	};

unsigned short procs_no = 0;
unsigned short procs_per_core = 0;
unsigned short fans_no = 0;

struct CORE_TEMP_INFO
{
	string label_file_path;
	string crit_file_path;
	string input_file_path;
	unsigned short core_id;
	unsigned int critical;
	unsigned int input;
	CORE_TEMP_INFO ()
	{
		core_id = 0;
		critical = 0;
		input = 0;
	}
};

struct FAN_INFO
{
	unsigned short fan_id;
	string input_file_path;
	short min_speed;
	short max_speed;
	short current_speed;
	short previous_speed;
    float display_speed;
    short current_interval;
    float min_interval;
    unsigned short force_interval;
	FAN_INFO ()
	{
		fan_id = 0;
		min_speed = 0x7FFF;
		max_speed = 0;
		current_speed = 0;
		previous_speed = 0;
        display_speed = 0.0;
        current_interval = 0;
        min_interval = 600;
        force_interval = 0;
	}
};

struct FAN_SPEED_MONITOR
{
	DIGITAL_DISPLAY speed_bar;
	DIGITAL_DISPLAY speed_value;
	FAN_INFO fan_speed_info;
};

void * autostart ( void * _argument )
{
	THREAD_ARGUMENT * thread_argument = ( THREAD_ARGUMENT * ) _argument;
	ifstream temp_data_file ( "temp_data" );
	if ( temp_data_file.is_open () )
	{
		temp_data_file.close ();
	}
	else
		system ( "find / -name \"temp[^_]_label\" 1> temp_data 2> /dev/null" );
	vector < CORE_TEMP_INFO > * core_temp_info_vector =
		( vector < CORE_TEMP_INFO > * ) thread_argument->structures [ 0 ];
	temp_data_file.open ( "temp_data" );
	while ( ! temp_data_file.eof () )
	{
		CORE_TEMP_INFO core_temp_info;
		string temp_str;
		temp_data_file >> temp_str;
		core_temp_info.label_file_path = temp_str;
		{
			ifstream label_file ( core_temp_info.label_file_path );
			if ( ! label_file.is_open () )
				continue;
			string label;
			string core_id_str;
			label_file >> label >> core_id_str;
			label_file.close ();
			long find_pos = label.find ( "ore", 0 );
			if ( find_pos == -1 )
				continue;
			core_temp_info.core_id = atoi ( core_id_str.c_str () );
		}
		temp_str = temp_str.substr ( 0, temp_str.length () - 5 );
		core_temp_info.crit_file_path = temp_str + "crit";
		core_temp_info.input_file_path = temp_str + "input";
		{
			ifstream crit_file ( core_temp_info.crit_file_path );
			if ( ! crit_file.is_open () )
				continue;
			crit_file >> core_temp_info.critical;
			crit_file.close ();
			core_temp_info.critical /= 1000;
			core_temp_info.critical -= 20;
		}
		core_temp_info_vector->push_back ( core_temp_info );
	}
	temp_data_file.close ();
    while ( core_temp_info_vector->size() == 0 )
        usleep ( 100000 );
	procs_per_core = procs_no / core_temp_info_vector->size ();
	ifstream fan_data_file ( "fan_data" );
	if ( fan_data_file.is_open () )
	{
		fan_data_file.close ();
	}
	else
		system ( "find / -name \"fan[^_]_input\" 1> fan_data 2> /dev/null" );
	vector < FAN_SPEED_MONITOR > * fan_speed_monitor_vector =
		( vector < FAN_SPEED_MONITOR > * ) thread_argument->structures [ 1 ];
	fan_data_file.open ( "fan_data" );
	while ( ! fan_data_file.eof () )
	{
		FAN_SPEED_MONITOR fan_speed_monitor;
		string temp_str;
		fan_data_file >> temp_str;
		if ( temp_str == "" )
			break;
		fan_speed_monitor.fan_speed_info.input_file_path = temp_str;
		temp_str = temp_str.substr ( 0, temp_str.length () - 6 );
		temp_str = temp_str.substr ( temp_str.length () - 1,
									 temp_str.length () );
		fan_speed_monitor.fan_speed_info.fan_id = atoi ( temp_str.c_str () );
		fan_speed_monitor_vector->push_back ( fan_speed_monitor );
	}
	fan_data_file.close ();
	DIGITAL_IMAGE * blue_fan_icon = ( DIGITAL_IMAGE * )
		thread_argument->classes [ 0 ];
	DIGITAL_IMAGE * fan_speed = ( DIGITAL_IMAGE * )
		thread_argument->classes [ 1 ];
	DIGITAL_IMAGE * blue_digit = ( DIGITAL_IMAGE * )
		thread_argument->classes [ 2 ];
	DIGITAL_IMAGE * fan_speed_delta = ( DIGITAL_IMAGE * )
		thread_argument->classes [ 3 ];
	THREAD_ARGUMENT * argument_clock_100ms = ( THREAD_ARGUMENT * )
		thread_argument->structures [ 2 ];
	for ( auto & fan_speed_monitor : * fan_speed_monitor_vector )
	{
		fan_speed_monitor.speed_bar.Create ( 2 );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 0, ' ', 0 );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 0, '#', 2 );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 1, ' ', 0x0800 );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 1, '0', 0x0002 );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 1, '1', 0x0006 );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 1, '2', 0x000E );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 1, '3', 0x001E );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 1, '4', 0x003E );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 1, '5', 0x007E );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 1, '6', 0x00FE );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 1, '7', 0x01FE );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 1, '8', 0x03FE );
		fan_speed_monitor.speed_bar.AddTypeDefinition ( 1, '9', 0x07FE );
		fan_speed_monitor.speed_bar.AddImage ( 0, blue_fan_icon );
		fan_speed_monitor.speed_bar.AddImage ( 1, fan_speed );
		fan_speed_monitor.speed_value.Create ( 2 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 0, ' ', 0 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 0, '+', 2 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 0, '-', 4 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 1, '0', 2 + 4 + 8
														  + 32 + 64 + 128 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 1, '1', 8 + 64 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 1, '2', 2 + 8 + 16
																  + 32 + 128 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 1, '3', 2 + 8 + 16
																  + 64 + 128 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 1, '4', 4 + 8 + 16
																  + 64 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 1, '5', 2 + 4 + 16
																  + 64 + 128 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 1, '6', 2 + 4 + 16
															  + 32 + 64 + 128 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 1, '7', 2 + 8 + 64 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 1, '8', 2 + 4 + 8
													  + 16 + 32 + 64 + 128 );
		fan_speed_monitor.speed_value.AddTypeDefinition ( 1, '9', 2 + 4 + 8
															  + 16 + 64 + 128 );
		fan_speed_monitor.speed_value.AddImage ( 1, blue_digit );
		fan_speed_monitor.speed_value.AddImage ( 0, fan_speed_delta );
		fan_speed_monitor.speed_value.AddImage ( 1, blue_digit );
		fan_speed_monitor.speed_value.AddImage ( 1, blue_digit );
		fan_speed_monitor.speed_value.AddImage ( 1, blue_digit );
		fan_speed_monitor.speed_value.AddImage ( 1, blue_digit );
		argument_clock_100ms->digital_displays
			.push_back ( & fan_speed_monitor.speed_bar );
		argument_clock_100ms->digital_displays
			.push_back ( & fan_speed_monitor.speed_value );
		fans_no++;
	}
	pthread_exit ( 0 );
}

void * clock_500ms ( void * _argument )
{
	THREAD_ARGUMENT * thread_argument = ( THREAD_ARGUMENT * ) _argument;
	unsigned long long clocks = 0;
	while ( ! thread_argument->stop_signal )
	{
		unsigned short object_id = 0;
		for ( auto & digital_display : thread_argument->digital_displays )
		{
			switch ( object_id )
			{
				case 0:
				{
					time_t system_time;
					struct tm * time_info;
					char temp_buffer [ 23 ] = { 0 };
					time ( & system_time );
					time_info = localtime ( & system_time );
					if ( ( clocks % 2 ) == 0 )
						strftime ( temp_buffer, sizeof ( temp_buffer ),
								   "#%d.%m.%Y #%H:%M:%S", time_info );
					else
						strftime ( temp_buffer, sizeof ( temp_buffer ),
								   "#%d.%m.%Y #%H %M %S", time_info );
					string temp_string ( temp_buffer );
					digital_display->SetString ( temp_string );
					break;
				}
				case 1:
				{
					time_t system_time;
					struct tm * time_info;
					char temp_buffer [ 23 ] = { 0 };
					time ( & system_time );
					time_info = localtime ( & system_time );
					if ( ( clocks % 2 ) == 0 )
						strftime ( temp_buffer, sizeof ( temp_buffer ),
								   "<%s", time_info );
					else
						strftime ( temp_buffer, sizeof ( temp_buffer ),
								   ">%s", time_info );
					string temp_string ( temp_buffer );
					digital_display->SetString ( temp_string );
					break;
				}
				default:
					break;
			}
			object_id++;
		}
		usleep ( 500000 );
		clocks++;
	}
	pthread_exit ( 0 );
}

struct PROC_INFO
{
	unsigned long long user;
	unsigned long long nice;
	unsigned long long system;
	unsigned long long idle;
	unsigned long long iowait;
	unsigned long long irq;
	unsigned long long softirq;
};

void ReadProcStat ( PROC_INFO * proc_info )
{
	char file_path [] = "/proc/stat";
	ifstream file ( file_path );
	string temp_str;
	for ( int i = 0; i < 11; i++ )
		file >> temp_str;
	for ( unsigned short proc_id = 0; proc_id < procs_no; proc_id++ )
	{
		file >> temp_str;
		PROC_INFO & t = proc_info [ proc_id ];
		file >> t.user >> t.nice >> t.system >> t.idle >> t.iowait >> t.irq
			 >> t.softirq;
		file >> temp_str >> temp_str >> temp_str;
	}
	file.close ();
}

void CalculateCoreDelta ( PROC_INFO * _a, PROC_INFO * _b, PROC_INFO * _r )
{
	for ( unsigned short proc_id = 0; proc_id < procs_no; proc_id++ )
	{
		PROC_INFO & a = _a [ proc_id ];
		PROC_INFO & b = _b [ proc_id ];
		PROC_INFO & r = _r [ proc_id ];
		r.user = a.user - b.user;
		r.nice = a.nice - b.nice;
		r.system = a.system - b.system;
		r.idle = a.idle - b.idle;
		r.iowait = a.iowait - b.iowait;
		r.irq = a.irq - b.irq;
		r.softirq = a.softirq - b.softirq;
	}
}

void ReadCoreTemperatures ( vector < CORE_TEMP_INFO > & _core_temp_info )
{
	for ( auto & core : _core_temp_info )
	{
		ifstream input_file ( core.input_file_path );
		if ( ! input_file.is_open () )
			continue;
		input_file >> core.input;
		input_file.close ();
		core.input /= 1000;
	}
}

void ReadFanSpeeds ( vector < FAN_SPEED_MONITOR > & _fan_speed_monitors )
{
	for ( auto & fan_speed_monitor : _fan_speed_monitors )
	{
		FAN_INFO & fan_info = fan_speed_monitor.fan_speed_info;
		ifstream input_file ( fan_info.input_file_path );
		if ( ! input_file.is_open () )
			continue;
        short input_value = 0;
        input_file >> input_value;
        input_file.close ();
        if ( input_value == fan_info.current_speed
                || abs ( fan_info.current_speed - input_value )
                   > 2 * ( fan_info.max_speed != 0 ? fan_info.max_speed
                                                   : 2048 ) )
        {
            fan_info.current_interval++;
            continue;
        }
        if ( fan_info.previous_speed != 0 )
        {
            if ( fan_info.current_interval == fan_info.min_interval )
                fan_info.force_interval++;
            if ( fan_info.current_interval < fan_info.min_interval )
            {
                if ( fan_info.force_interval > 0 )
                    fan_info.force_interval--;
                else
                    fan_info.min_interval = fan_info.current_interval;
            }
        }
        fan_info.current_interval = 0;
		fan_info.previous_speed = fan_info.current_speed;
        fan_info.display_speed = fan_info.previous_speed;
        fan_info.current_speed = input_value;
		if ( fan_info.current_speed < fan_info.min_speed
			&& fan_info.current_speed != 0 )
			fan_info.min_speed = fan_info.current_speed;
		if ( fan_info.current_speed > fan_info.max_speed
			&& fan_info.current_speed < 10000 )
			fan_info.max_speed = fan_info.current_speed;
	}
}

void * clock_100ms ( void * _argument )
{
	THREAD_ARGUMENT * thread_argument = ( THREAD_ARGUMENT * ) _argument;
	vector < CORE_TEMP_INFO > * core_temp_info =
		( vector < CORE_TEMP_INFO > * ) thread_argument->structures [ 0 ];
	vector < FAN_SPEED_MONITOR > * fan_speed_monitors =
		( vector < FAN_SPEED_MONITOR > * ) thread_argument->structures [ 1 ];
	unsigned long long clocks = 0;
	PROC_INFO * proc_info_main = new PROC_INFO [ procs_no ];
	PROC_INFO * proc_info_temp = new PROC_INFO [ procs_no ];
	PROC_INFO * proc_info_main_1s = new PROC_INFO [ procs_no ];
	PROC_INFO * proc_info_temp_1s = new PROC_INFO [ procs_no ];
	ReadProcStat ( proc_info_main );
	ReadProcStat ( proc_info_main_1s );
	while ( ! thread_argument->stop_signal )
	{
		ReadProcStat ( proc_info_temp );
		CalculateCoreDelta ( proc_info_temp, proc_info_main, proc_info_main );
		ReadFanSpeeds ( * fan_speed_monitors );
		if ( ( clocks % 10 ) == 0 )
		{
			ReadProcStat ( proc_info_temp_1s );
			CalculateCoreDelta ( proc_info_temp_1s, proc_info_main_1s,
								 proc_info_main_1s );
			ReadCoreTemperatures ( * core_temp_info );
		}
		unsigned short object_id = 0;
		for ( auto & digital_display : thread_argument->digital_displays )
		{
			unsigned short next_id_limit = procs_no;
			if ( object_id < next_id_limit )
			{
				string proc_info_string;
				{
					PROC_INFO & proc_info = proc_info_main [ object_id ];
					unsigned long long total = proc_info.user + proc_info.nice +
						proc_info.system + proc_info.idle + proc_info.iowait +
						proc_info.irq + proc_info.softirq;
					unsigned short system = 0;
					unsigned short user = 0;
					unsigned short wait = 0;
					if ( total != 0 )
					{
						system = 100 * ( proc_info.system
								+ proc_info.irq + proc_info.softirq ) / total;
						user = 100 * ( proc_info.user + proc_info.nice )
											  / total;
						wait = 100 * ( proc_info.iowait )
											  / total;
					}
					if ( system == 0 && user == 0 && wait == 0 )
						proc_info_string += " ";
					else
						proc_info_string += "#";
					if ( object_id < 10 )
						proc_info_string += "0";
					proc_info_string += to_string ( object_id );
					for ( unsigned short i = 0; i < 10; i++ )
					{
						unsigned short temp_value = 0;
						if ( system > i * 10 )
							temp_value += 1;
						if ( user > i * 10 )
							temp_value += 2;
						if ( wait > i * 10 )
							temp_value += 4;
						proc_info_string += to_string ( temp_value );
					}
				}
				{
					PROC_INFO & proc_info = proc_info_main_1s [ object_id ];
					unsigned long long total = proc_info.user + proc_info.nice +
					   proc_info.system + proc_info.idle + proc_info.iowait +
						  proc_info.irq + proc_info.softirq;
					unsigned short system = 0;
					unsigned short user = 0;
					unsigned short wait = 0;
					if ( total != 0 )
					{
						system = 100 * ( proc_info.system
								 + proc_info.irq + proc_info.softirq ) / total;
						user = 100 * ( proc_info.user + proc_info.nice )
							   / total;
						wait = 100 * ( proc_info.iowait )
							   / total;
						if ( system == 100 )
							system--;
						if ( user == 100 )
							user--;
						if ( wait == 100 )
							wait--;
					}
					if ( system > 0 )
					{
						if ( system < 10 )
							proc_info_string += " ";
						proc_info_string += to_string ( system );
					}
					else
						proc_info_string += "  ";
					if ( user > 0 )
					{
						if ( user < 10 )
							proc_info_string += " ";
						proc_info_string += to_string ( user );
					}
					else
						proc_info_string += "  ";
					if ( wait > 0 )
					{
						if ( wait < 10 )
							proc_info_string += " ";
						proc_info_string += to_string ( wait );
					}
					else
						proc_info_string += "  ";
					unsigned short core_temp = 0;
					bool alarm = false;
					if ( procs_per_core != 0 )
					{
						for ( auto & core : * core_temp_info )
							if ( object_id / procs_per_core == core.core_id )
							{
								core_temp = core.input;
								if ( core.input >= core.critical )
									alarm = true;
								break;
							}
					}
					if ( core_temp != 0 )
					{
						if ( core_temp < 100 )
							proc_info_string += " ";
						if ( core_temp < 10 )
							proc_info_string += " ";
						proc_info_string += to_string ( core_temp );
						if ( alarm && ( ( ( clocks / 2 ) % 2 ) == 0 ) )
							proc_info_string += "!";
						else
							proc_info_string += " ";
					}
					else
						proc_info_string += "   !";
				}
				digital_display->SetString ( proc_info_string );
			}
			object_id++;
		}
		for ( auto & fan_speed_monitor : * fan_speed_monitors )
		{
			FAN_INFO & fan_info = fan_speed_monitor.fan_speed_info;
            {
                if ( fan_info.current_speed > fan_info.previous_speed )
                {
                    if ( fan_info.display_speed < fan_info.current_speed )
                        fan_info.display_speed += ( fan_info.current_speed
                         - fan_info.previous_speed ) / fan_info.min_interval;
                    else
                        fan_info.display_speed = fan_info.current_speed;
                }
                else
                {
                    if ( fan_info.display_speed > fan_info.current_speed )
                        fan_info.display_speed += ( fan_info.current_speed
                          - fan_info.previous_speed ) / fan_info.min_interval;
                    else
                        fan_info.display_speed = fan_info.current_speed;
                }
            }
			{
				string fan_string = "#";
				if ( fan_info.current_speed == 0
					 || fan_info.current_speed >= 10000
                        || fan_info.force_interval == 0 )
					fan_string += " ";
				else
				{
					short fan_speed = 0;
					if ( fan_info.max_speed - fan_info.min_speed != 0 )
						fan_speed = 10 * ( fan_info.display_speed
						- fan_info.min_speed ) / ( fan_info.max_speed
												   - fan_info.min_speed );
					if ( fan_speed == 10 )
						fan_speed--;
					fan_string += to_string ( fan_speed );
				}
				fan_speed_monitor.speed_bar.SetString ( fan_string );
			}
			{
				string fan_string = to_string ( fan_info.fan_id );
				if ( fan_info.current_speed - fan_info.previous_speed
                     > 16 * fan_info.min_interval
                     && fan_info.display_speed != fan_info.current_speed )
					fan_string += "+";
				else
				{
					if ( fan_info.current_speed - fan_info.previous_speed
						 < -16 * fan_info.min_interval
                         && fan_info.current_speed != 0
                         && fan_info.display_speed != fan_info.current_speed )
						fan_string += "-";
					else
						fan_string += " ";
				}
				if ( fan_info.force_interval > 0
                     && fan_info.display_speed < 10000 )
				{
					if ( fan_info.display_speed < 1000 )
						fan_string += " ";
					if ( fan_info.display_speed < 100 )
						fan_string += " ";
					if ( fan_info.display_speed < 10 )
						fan_string += " ";
					fan_string
                            += to_string ( ( short ) fan_info.display_speed );
				}
				else
				{
					fan_string += "   0";
				}
				fan_speed_monitor.speed_value.SetString ( fan_string );
			}
		}
		{
			PROC_INFO * temp = proc_info_temp;
			proc_info_temp = proc_info_main;
			proc_info_main = temp;
		}
		if ( ( clocks % 10 ) == 9 )
		{
			PROC_INFO * temp = proc_info_temp_1s;
			proc_info_temp_1s = proc_info_main_1s;
			proc_info_main_1s = temp;
		}
		usleep ( 100000 );
		clocks++;
	}
	delete [ ] proc_info_main;
	delete [ ] proc_info_temp;
	pthread_exit ( 0 );
}

int main ()
{
	sf::VideoMode desktop_mode = sf::VideoMode::getDesktopMode ();
	sf::Vector2i status_bar_pos
		= sf::Vector2i ( 1920 + 256, desktop_mode.height - 18 );
	sf::VideoMode status_bar_size
		= sf::VideoMode ( desktop_mode.width - 256 - 1920, 18 );

	sf::RenderWindow status_bar;
	status_bar.create ( status_bar_size, "", sf::Style::None );
	sf::WindowHandle status_bar_handle = status_bar.getSystemHandle ();

	status_bar.setPosition ( status_bar_pos );
	status_bar.setVerticalSyncEnabled ( true );
	status_bar.setFramerateLimit ( 10 );

	sf::Vector2i default_mouse_position = sf::Mouse::getPosition ();
	sf::Mouse::setPosition ( status_bar_pos );
	system ( "i3-msg sticky toggle" );
	sf::Mouse::setPosition ( default_mouse_position );

	MATRIX < unsigned short > space_matrix;
	space_matrix.Create ( 8, 4 );
	space_matrix = space_def;
	MATRIX < unsigned short > digit_matrix;
	digit_matrix.Create ( 8, 5 );
	digit_matrix = digit_def;
	MATRIX < unsigned short > colon_matrix;
	colon_matrix.Create ( 8, 2 );
	colon_matrix = colon_def;
	MATRIX < unsigned short > clock_icon_matrix;
	clock_icon_matrix.Create ( 8, 8 );
	clock_icon_matrix = clock_icon_def;
	MATRIX < unsigned short > dot_matrix;
	dot_matrix.Create ( 8, 2 );
	dot_matrix = dot_def;
	MATRIX < unsigned short > date_icon_matrix;
	date_icon_matrix.Create ( 8, 8 );
	date_icon_matrix = date_icon_def;
	MATRIX < unsigned short > system_time_icon_matrix;
	system_time_icon_matrix.Create ( 8, 8 );
	system_time_icon_matrix = system_time_icon_def;
	MATRIX < unsigned short > proc_usage_matrix;
	proc_usage_matrix.Create ( 8, 2 );
	proc_usage_matrix = proc_usage_def;
	MATRIX < unsigned short > temp_alarm_matrix;
	temp_alarm_matrix.Create ( 8, 6 );
	temp_alarm_matrix = temp_alarm_def;
	MATRIX < unsigned short > proc_icon_matrix;
	proc_icon_matrix.Create ( 8, 8 );
	proc_icon_matrix = proc_icon_def;
	MATRIX < unsigned short > fan_speed_matrix;
	fan_speed_matrix.Create ( 8, 20 );
	fan_speed_matrix = fan_speed_def;
	MATRIX < unsigned short > fan_speed_delta_matrix;
	fan_speed_delta_matrix.Create ( 8, 4 );
	fan_speed_delta_matrix = fan_speed_delta_def;
	MATRIX < unsigned short > fan_icon_matrix;
	fan_icon_matrix.Create ( 8, 8 );
	fan_icon_matrix = fan_icon_def;

	DIGITAL_IMAGE white_space;
	white_space.Create ( 1 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		white_space.DefineLayer ( 1, black, black );
	}
	white_space.SetMatrix ( & space_matrix );
	DIGITAL_IMAGE cyan_digit;
	cyan_digit.Create ( 8 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		cyan_digit.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 0, 31, 31 );
		RGB_COLOR on ( 0, 255, 255 );
		unsigned int id = 2;
		for ( unsigned short i = 1; i < 8; i++ )
		{
			cyan_digit.DefineLayer ( id, off, on );
			id = id << 1;
		}
	}
	cyan_digit.SetMatrix ( & digit_matrix );
	DIGITAL_IMAGE cyan_colon;
	cyan_colon.Create ( 2 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		cyan_colon.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 0, 31, 31 );
		RGB_COLOR on ( 0, 255, 255 );
		cyan_colon.DefineLayer ( 2, off, on );
	}
	cyan_colon.SetMatrix ( & colon_matrix );
	DIGITAL_IMAGE cyan_clock_icon;
	cyan_clock_icon.Create ( 2 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		cyan_clock_icon.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 0, 31, 31 );
		RGB_COLOR on ( 0, 255, 255 );
		cyan_clock_icon.DefineLayer ( 2, off, on );
	}
	cyan_clock_icon.SetMatrix ( & clock_icon_matrix );
	DIGITAL_IMAGE cyan_dot;
	cyan_dot.Create ( 2 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		cyan_dot.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 0, 31, 31 );
		RGB_COLOR on ( 0, 255, 255 );
		cyan_dot.DefineLayer ( 2, off, on );
	}
	cyan_dot.SetMatrix ( & dot_matrix );
	DIGITAL_IMAGE cyan_date_icon;
	cyan_date_icon.Create ( 2 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		cyan_date_icon.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 0, 31, 31 );
		RGB_COLOR on ( 0, 255, 255 );
		cyan_date_icon.DefineLayer ( 2, off, on );
	}
	cyan_date_icon.SetMatrix ( & date_icon_matrix );
	DIGITAL_IMAGE cyan_system_time_icon;
	cyan_system_time_icon.Create ( 4 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		cyan_system_time_icon.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 0, 31, 31 );
		RGB_COLOR on ( 0, 255, 255 );
		RGB_COLOR on2 ( 0, 191, 191 );
		cyan_system_time_icon.DefineLayer ( 2, off, on );
		unsigned int id = 4;
		for ( unsigned short i = 2; i < 4; i++ )
		{
			cyan_system_time_icon.DefineLayer ( id, black, on2 );
			id = id << 1;
		}
	}
	cyan_system_time_icon.SetMatrix ( & system_time_icon_matrix );
	DIGITAL_IMAGE red_digit;
	red_digit.Create ( 8 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		red_digit.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 31, 0, 0 );
		RGB_COLOR on ( 255, 0, 0 );
		unsigned int id = 2;
		for ( unsigned short i = 1; i < 8; i++ )
		{
			red_digit.DefineLayer ( id, off, on );
			id = id << 1;
		}
	}
	red_digit.SetMatrix ( & digit_matrix );
	DIGITAL_IMAGE green_digit;
	green_digit.Create ( 8 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		green_digit.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 0, 31, 0 );
		RGB_COLOR on ( 0, 255, 0 );
		unsigned int id = 2;
		for ( unsigned short i = 1; i < 8; i++ )
		{
			green_digit.DefineLayer ( id, off, on );
			id = id << 1;
		}
	}
	green_digit.SetMatrix ( & digit_matrix );
	DIGITAL_IMAGE blue_digit;
	blue_digit.Create ( 8 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		blue_digit.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 0, 0, 31 );
		RGB_COLOR on ( 0, 0, 255 );
		unsigned int id = 2;
		for ( unsigned short i = 1; i < 8; i++ )
		{
			blue_digit.DefineLayer ( id, off, on );
			id = id << 1;
		}
	}
	blue_digit.SetMatrix ( & digit_matrix );
	DIGITAL_IMAGE proc_usage;
	proc_usage.Create ( 4 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		proc_usage.DefineLayer ( 1, black, black );
		RGB_COLOR red_off ( 31, 0, 0 );
		RGB_COLOR red_on ( 255, 0, 0 );
		RGB_COLOR green_off ( 0, 31, 0 );
		RGB_COLOR green_on ( 0, 255, 0 );
		RGB_COLOR blue_off ( 0, 0, 31 );
		RGB_COLOR blue_on ( 0, 0, 255 );
		proc_usage.DefineLayer ( 2, red_off, red_on );
		proc_usage.DefineLayer ( 4, green_off, green_on );
		proc_usage.DefineLayer ( 8, blue_off, blue_on );
	}
	proc_usage.SetMatrix ( & proc_usage_matrix );
	DIGITAL_IMAGE yellow_digit;
	yellow_digit.Create ( 8 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		yellow_digit.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 31, 31, 0 );
		RGB_COLOR on ( 255, 255, 0 );
		unsigned int id = 2;
		for ( unsigned short i = 1; i < 8; i++ )
		{
			yellow_digit.DefineLayer ( id, off, on );
			id = id << 1;
		}
	}
	yellow_digit.SetMatrix ( & digit_matrix );
	DIGITAL_IMAGE temp_alarm;
	temp_alarm.Create ( 2 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		temp_alarm.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 31, 0, 0 );
		RGB_COLOR on ( 255, 0, 0 );
		temp_alarm.DefineLayer ( 2, off, on );
	}
	temp_alarm.SetMatrix ( & temp_alarm_matrix );
	DIGITAL_IMAGE green_proc_icon;
	green_proc_icon.Create ( 3 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		green_proc_icon.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 0, 31, 0 );
		RGB_COLOR on ( 0, 255, 0 );
		RGB_COLOR on2 ( 0, 191, 0 );
		green_proc_icon.DefineLayer ( 2, off, on );
		green_proc_icon.DefineLayer ( 4, black, on2 );
	}
	green_proc_icon.SetMatrix ( & proc_icon_matrix );
	DIGITAL_IMAGE fan_speed;
	fan_speed.Create ( 12 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		fan_speed.DefineLayer ( 1, black, black );
		RGB_COLOR blue_off ( 0, 0, 31 );
		RGB_COLOR blue_on ( 0, 0, 255 );
		RGB_COLOR red_off ( 31, 0, 0 );
		RGB_COLOR red_on ( 255, 0, 0 );
		unsigned int id = 2;
		for ( unsigned short i = 1; i < 11; i++ )
		{
			fan_speed.DefineLayer ( id, blue_off, blue_on );
			id = id << 1;
		}
		fan_speed.DefineLayer ( id, red_off, red_on );
	}
	fan_speed.SetMatrix ( & fan_speed_matrix );
	DIGITAL_IMAGE fan_speed_delta;
	fan_speed_delta.Create ( 3 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		fan_speed_delta.DefineLayer ( 1, black, black );
		RGB_COLOR red_off ( 31, 0, 0 );
		RGB_COLOR red_on ( 255, 0, 0 );
		RGB_COLOR green_off ( 0, 31, 0 );
		RGB_COLOR green_on ( 0, 255, 0 );
		fan_speed_delta.DefineLayer ( 2, green_off, green_on );
		fan_speed_delta.DefineLayer ( 4, red_off, red_on );
	}
	fan_speed_delta.SetMatrix ( & fan_speed_delta_matrix );
	DIGITAL_IMAGE blue_fan_icon;
	blue_fan_icon.Create ( 2 );
	{
		RGB_COLOR black ( 0, 0, 0 );
		blue_fan_icon.DefineLayer ( 1, black, black );
		RGB_COLOR off ( 0, 0, 31 );
		RGB_COLOR on ( 0, 0, 255 );
		blue_fan_icon.DefineLayer ( 2, off, on );
	}
	blue_fan_icon.SetMatrix ( & fan_icon_matrix );

	DIGITAL_DISPLAY time_and_date_display;
	time_and_date_display.Create ( 3 );
	{
		time_and_date_display.AddTypeDefinition ( 0, ' ', 0 );
		time_and_date_display.AddTypeDefinition ( 0, ':', 2 );
		time_and_date_display.AddTypeDefinition ( 0, '.', 2 );
		time_and_date_display.AddTypeDefinition ( 1, '0', 2 + 4 + 8 + 32 + 64
														  + 128 );
		time_and_date_display.AddTypeDefinition ( 1, '1', 8 + 64 );
		time_and_date_display.AddTypeDefinition ( 1, '2', 2 + 8 + 16 + 32
														  + 128 );
		time_and_date_display.AddTypeDefinition ( 1, '3', 2 + 8 + 16 + 64
														  + 128 );
		time_and_date_display.AddTypeDefinition ( 1, '4', 4 + 8 + 16 + 64 );
		time_and_date_display.AddTypeDefinition ( 1, '5', 2 + 4 + 16 + 64
														  + 128 );
		time_and_date_display.AddTypeDefinition ( 1, '6', 2 + 4 + 16 + 32
														  + 64 + 128 );
		time_and_date_display.AddTypeDefinition ( 1, '7', 2 + 8 + 64 );
		time_and_date_display.AddTypeDefinition ( 1, '8', 2 + 4 + 8 + 16 + 32
														  + 64 + 128 );
		time_and_date_display.AddTypeDefinition ( 1, '9', 2 + 4 + 8 + 16 + 64
														  + 128 );
		time_and_date_display.AddTypeDefinition ( 2, ' ', 0 );
		time_and_date_display.AddTypeDefinition ( 2, '#', 2 );
		time_and_date_display.AddImage ( 2, & cyan_date_icon );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 0, & cyan_dot );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 0, & cyan_dot );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 2, & white_space );
		time_and_date_display.AddImage ( 2, & cyan_clock_icon );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 0, & cyan_colon );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 0, & cyan_colon );
		time_and_date_display.AddImage ( 1, & cyan_digit );
		time_and_date_display.AddImage ( 1, & cyan_digit );
	}
	DIGITAL_DISPLAY time_details_display;
	time_details_display.Create ( 3 );
	{
		time_details_display.AddTypeDefinition ( 0, ' ', 0 );
		time_details_display.AddTypeDefinition ( 0, '.', 2 );
		time_details_display.AddTypeDefinition ( 1, '0', 2 + 4 + 8 + 32 + 64
														 + 128 );
		time_details_display.AddTypeDefinition ( 1, '1', 8 + 64 );
		time_details_display.AddTypeDefinition ( 1, '2', 2 + 8 + 16 + 32
														 + 128 );
		time_details_display.AddTypeDefinition ( 1, '3', 2 + 8 + 16 + 64
														 + 128 );
		time_details_display.AddTypeDefinition ( 1, '4', 4 + 8 + 16 + 64 );
		time_details_display.AddTypeDefinition ( 1, '5', 2 + 4 + 16 + 64
														 + 128 );
		time_details_display.AddTypeDefinition ( 1, '6', 2 + 4 + 16 + 32
														 + 64 + 128 );
		time_details_display.AddTypeDefinition ( 1, '7', 2 + 8 + 64 );
		time_details_display.AddTypeDefinition ( 1, '8', 2 + 4 + 8 + 16 + 32
														 + 64 + 128 );
		time_details_display.AddTypeDefinition ( 1, '9', 2 + 4 + 8 + 16 + 64
														 + 128 );
		time_details_display.AddTypeDefinition ( 2, ' ', 0 );
		time_details_display.AddTypeDefinition ( 2, '#', 2 );
		time_details_display.AddTypeDefinition ( 2, '<', 2 + 4 );
		time_details_display.AddTypeDefinition ( 2, '>', 2 + 8 );
		time_details_display.AddImage ( 2, & cyan_system_time_icon );
		for ( int i = 0; i < 10; i++ )
			time_details_display.AddImage ( 1, & cyan_digit );
	}
	{
		char stat_file_path [ ] = "/proc/stat";
		ifstream stat_file ( stat_file_path );
		unsigned short values_num = 0;
		{
			string temp_str;
			while ( true )
			{
				stat_file >> temp_str;
				short find_pos = temp_str.find ( "intr", 0 );
				if ( find_pos > -1 )
					break;
				values_num++;
			}
		}
		stat_file.close ();
		values_num -= 11;
		procs_no = values_num / 11;
	}

	DIGITAL_DISPLAY * proc_details_display = new DIGITAL_DISPLAY [ procs_no ];
	for ( unsigned short proc_id = 0; proc_id < procs_no; proc_id++ )
	{
		proc_details_display [ proc_id ].Create ( 4 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 0, ' ', 2 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 0, '#', 6 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 1, '0', 2 + 4 + 8
															 + 32 + 64 + 128 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 1, '1', 8 + 64 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 1, '2', 2 + 8 + 16
																 + 32 + 128 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 1, '3', 2 + 8 + 16
																 + 64 + 128 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 1, '4', 4 + 8 + 16
																	 + 64 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 1, '5', 2 + 4 + 16
																 + 64 + 128 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 1, '6', 2 + 4 + 16
															 + 32 + 64 + 128 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 1, '7', 2 + 8
																	 + 64 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 1, '8', 2 + 4 + 8
															 + 16 + 32 + 64
																	 + 128 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 1, '9', 2 + 4 + 8
															 + 16 + 64 + 128 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 2, '0', 0 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 2, '1', 2 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 2, '2', 4 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 2, '3', 6 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 2, '4', 8 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 2, '5', 10 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 2, '6', 12 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 2, '7', 14 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 3, ' ', 0 );
		proc_details_display [ proc_id ].AddTypeDefinition ( 3, '!', 2 );
		proc_details_display [ proc_id ].AddImage ( 0, & green_proc_icon );
		proc_details_display [ proc_id ].AddImage ( 1, & green_digit );
		proc_details_display [ proc_id ].AddImage ( 1, & green_digit );
		for ( int i = 0; i < 10; i++ )
			proc_details_display [ proc_id ].AddImage ( 2, & proc_usage );
		proc_details_display [ proc_id ].AddImage ( 1, & red_digit );
		proc_details_display [ proc_id ].AddImage ( 1, & red_digit );
		proc_details_display [ proc_id ].AddImage ( 1, & green_digit );
		proc_details_display [ proc_id ].AddImage ( 1, & green_digit );
		proc_details_display [ proc_id ].AddImage ( 1, & blue_digit );
		proc_details_display [ proc_id ].AddImage ( 1, & blue_digit );
		proc_details_display [ proc_id ].AddImage ( 1, & yellow_digit );
		proc_details_display [ proc_id ].AddImage ( 1, & yellow_digit );
		proc_details_display [ proc_id ].AddImage ( 1, & yellow_digit );
		proc_details_display [ proc_id ].AddImage ( 3, & temp_alarm );
	}

	vector < CORE_TEMP_INFO > core_temps;
	vector < FAN_SPEED_MONITOR > fan_speed_monitors;


	THREAD_ARGUMENT argument_clock_500ms;
	argument_clock_500ms.digital_displays.push_back ( & time_and_date_display );
	argument_clock_500ms.digital_displays.push_back ( & time_details_display );
	pthread_t thread_clock_500ms;
	pthread_create ( & thread_clock_500ms, nullptr, clock_500ms,
					 & argument_clock_500ms );

	THREAD_ARGUMENT argument_clock_100ms;
	for ( unsigned short proc_id = 0; proc_id < procs_no; proc_id++ )
	argument_clock_100ms.digital_displays.push_back
		( proc_details_display + proc_id );
	argument_clock_100ms.structures.push_back ( & core_temps );
	argument_clock_100ms.structures.push_back ( & fan_speed_monitors );
	pthread_t thread_clock_100ms;
	pthread_create ( & thread_clock_100ms, nullptr, clock_100ms,
					 & argument_clock_100ms );

	THREAD_ARGUMENT argument_autostart;
	argument_autostart.structures.push_back ( & core_temps );
	argument_autostart.structures.push_back ( & fan_speed_monitors );
	argument_autostart.structures.push_back ( & argument_clock_100ms );
	argument_autostart.classes.push_back ( & blue_fan_icon );
	argument_autostart.classes.push_back ( & fan_speed );
	argument_autostart.classes.push_back ( & blue_digit );
	argument_autostart.classes.push_back ( & fan_speed_delta );
	pthread_t thread_autostart;
	pthread_create ( & thread_autostart, nullptr, autostart,
					 & argument_autostart );

	while ( status_bar.isOpen () )
	{
		sf::Event event;
		while ( status_bar.pollEvent ( event ) )
		{
			if ( event.type == sf::Event::Closed )
				status_bar.close ();
		}
		status_bar.clear ( sf::Color ( 0, 0, 0 ) );
		unsigned int pos = status_bar_size.width - 1;
//        unsigned int delta_pos = 196;
//        time_and_date_display.DrawDisplay ( status_bar,
//											pos - delta_pos, 2, 2 );
//        pos -= delta_pos;
//        delta_pos = 124;
//		time_details_display.DrawDisplay ( status_bar, pos - delta_pos, 2, 2 );
//        pos -= delta_pos;
//        delta_pos = 180;
//		for ( unsigned short proc_id = procs_no - 1; proc_id != 0xFFFF;
//			  proc_id-- )
//		{
//            proc_details_display [ proc_id ]
//                    .DrawDisplay ( status_bar, pos - delta_pos, 2, 2 );
//            pos -= delta_pos;
//		}
//        delta_pos = 64;
//		for ( auto & fan_speed_monitor : fan_speed_monitors )
//		{
//			fan_speed_monitor.speed_bar.DrawDisplay ( status_bar,
//													  pos - delta_pos, 2, 2 );
//            pos -= delta_pos;
//			fan_speed_monitor.speed_value.DrawDisplay ( status_bar,
//													  pos - delta_pos, 2, 2 );
//			pos -= delta_pos;
//		}
		unsigned int delta_pos = 98;
		time_and_date_display.DrawDisplay ( status_bar,
											pos - delta_pos, 10, 1 );
		time_details_display.DrawDisplay ( status_bar, pos - delta_pos, 1, 1 );
		pos -= delta_pos;
		delta_pos = 90;
		bool top = false;
		for ( unsigned short proc_id = procs_no - 1; proc_id != 0xFFFF;
			  proc_id-- )
		{
			if ( top )
				proc_details_display [ proc_id ]
					.DrawDisplay ( status_bar, pos - delta_pos, 1, 1 );
			else
				proc_details_display [ proc_id ]
					.DrawDisplay ( status_bar, pos - delta_pos, 10, 1 );
			if ( top )
			{
				pos -= delta_pos;
			}
			top = ! top;
		}
		if ( top )
			pos -= delta_pos;
		delta_pos = 32;
		for ( auto & fan_speed_monitor : fan_speed_monitors )
		{
			fan_speed_monitor.speed_bar.DrawDisplay ( status_bar,
													  pos - delta_pos, 1, 1 );
			fan_speed_monitor.speed_value.DrawDisplay ( status_bar,
													  pos - delta_pos, 10, 1 );
			pos -= delta_pos;
		}
		status_bar.display ();
	}

	argument_clock_500ms.stop_signal = true;
	argument_clock_100ms.stop_signal = true;
	delete [ ] proc_details_display;
	time_and_date_display.Destroy ();
	cyan_digit.Destroy ();
	digit_matrix.Destroy ();

	return 0;
}