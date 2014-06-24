#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <memory>
#include <fstream>
#include <iterator>
#include <iomanip>

using namespace std;


template < typename T, typename Char = char, typename traits = char_traits< Char > >
class infix_ostream_iterator : public iterator< output_iterator_tag, Char, traits >
{
	public:
	
		using char_type = Char;
		using traits_type = traits;
		using ostream_type = basic_ostream< char_type, traits_type >;
		using self = infix_ostream_iterator< T, char_type, traits_type >;
	
		infix_ostream_iterator( ostream_type& s, const string &delim = string() ) :
	
		output( &self::outputFirst ),
			os( s ),
			delimiter( delim ) {}

		self& operator = ( const T &item )
		{
			(this->*output)( item );
			return *this;
		}
	
		self& operator*() { return *this; }
	
		self& operator++() { return *this; }
	
		self& operator++(int) { return *this; }
	
	private:
	
		void outputFirst( const T &item )
		{
			os << item;
			output = &self::outputRest;
		}
		
		void outputRest( const T &item )
		{
			os << delimiter << item;
		}
		
		void (self::*output)(const T &);
		
		ostream_type &os;
		const string delimiter;
};

struct Tile
{
	uint8_t type;
	uint8_t number;
};

using Strings = vector< string >;
using Tiles = vector< Tile >;

Tiles init_Tiles()
{
	Tiles result;
	
	for ( int i = 1; i <= 13; ++i )
	{
		for ( char c = 'A'; c <= 'D'; ++c )
		{
			Tile t;
			t.number = i;
			t.type = c;
			result.push_back( t );
		}
	}
	
	return move( result );
}

struct Player
{
	string executable;
	Tiles inhand;
};

using Combinations = vector< Tiles >;

using Players = vector< Player >;

ostream& operator << ( ostream &str, const Tile &t )
{
	str << t.type;
	str << setw( 2 );
	str << setfill( '_' ) << static_cast< int >( t.number );
	return str;
}

ostream& operator << ( ostream &str, const Tiles &t )
{
	str << '[';
	copy( t.begin(), t.end(), infix_ostream_iterator< Tile >( str, "," ) );
	return str << ']';
}

ostream& operator << ( ostream &str, const Combinations &t )
{
	for ( auto &i : t )
	{
		str << i;
	}
	return str;
}

Tiles get_random_Tiles( Tiles &pool, size_t count )
{
	random_device rd;
	default_random_engine re( rd() );
	
	if ( pool.size() < count )
	{
		count = pool.size();
	}
	
	Tiles result;
	while ( count-- )
	{
		uniform_int_distribution< int > random( 0, pool.size() - 1 );
		auto q = random( re );
		auto i = pool.begin() + q;
		result.push_back( *i );
		pool.erase( i );
	}
	
	return move( result );
}

string callProcess( string exe )
{
	unique_ptr< FILE, function< decltype( pclose ) > > stream( popen( exe.c_str(), "r" ), pclose );
	
	string buffer;
	size_t total = 0;

	size_t amount = 0;
	do
	{
		buffer.resize( total + 64 );
		amount = fread( &buffer[ 0 ], 1, buffer.size(), stream.get() );
		total += amount;
		buffer.resize( total );

		cout << string( buffer.begin(), buffer.begin() + amount ) << endl;
	}
	while ( amount );
	
	return move( buffer );
}

void run_move( Player &player, Tiles &pool, Combinations &combinations )
{
	const string temporaryFileName( "/tmp/" + to_string( rand() ) + ".txt" );
	ofstream temporaryFile( temporaryFileName );
	temporaryFile << player.inhand << "\n\n" << combinations;
	temporaryFile.close();
	
	callProcess( player.executable + " < " + temporaryFileName );
	
}

template < typename T >
void run_game( const T &&executables )
{
	Tiles pool = init_Tiles();
	
	Players players;
	Combinations field;
	
	for ( auto &exe : executables )
	{
		Player p;
		p.executable = exe;
		p.inhand = get_random_Tiles( pool, 16 );
		players.push_back( p );
	}
	
	for ( auto &p : players )
	{
		run_move( p, pool, field );
	}
}

int main( int argc, char *argv[] )
{
	try
	{
		if ( argc < 2 )
		{
			throw runtime_error( "no clients specified" );
		}
		
		run_game( Strings( argv + 1, argv + argc ) );
	}
	catch ( const exception &err )
	{
		cerr << err.what() << endl;
		
		return 1;
	}
	
	return 0;
}