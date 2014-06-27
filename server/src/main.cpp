#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <memory>
#include <fstream>
#include <iterator>
#include <iomanip>
#include <sstream>

#include <unistd.h>
#include <fcntl.h>

using namespace std;

namespace color
{
	enum type
	{
		red = 1 << 0,
		yellow = 1 << 1,
		blue = 1 << 2,
		black = 1 << 3,
		mask = 0xF
	};
}

static const vector< color::type > colors = {
	color::red,
	color::yellow,
	color::blue,
	color::black
};

namespace number
{
	enum type
	{
		one = 1 << 4,
		two = 1 << 5,
		three = 1 << 6,
		four = 1 << 7,
		five = 1 << 8,
		six = 1 << 9,
		seven = 1 << 10,
		eight = 1 << 11,
		nine = 1 << 12,
		ten = 1 << 13,
		eleven = 1 << 14,
		twelve = 1 << 15,
		thirteen = 1 << 16,
		mask = 0x1FFF0
	};
}

static const vector< number::type > numbers = {
	number::one,
	number::two,
	number::three,
	number::four,
	number::five,
	number::six,
	number::seven,
	number::eight,
	number::nine,
	number::ten,
	number::eleven,
	number::twelve,
	number::thirteen
};

class Tile
{
	public:
	
	Tile( number::type n, color::type c ) :
			data_( n | c ) {}

		Tile() :
			data_( 0 ) {}
		
		bool operator == ( const Tile &rhs ) const
		{
			return data_ == rhs.data_;
		}
		
		bool operator < ( const Tile &rhs ) const
		{
			return data_ < rhs.data_;
		}
		
		bool valid() const
		{
			return ( data_ & color::mask ) && ( data_ & number::mask );
		}
	
	color::type color() const
	{
		return static_cast< color::type >( data_ & color::mask );
	}
	
	number::type number() const
	{
		return static_cast< number::type >( data_ & number::mask );
	}
	
	private:
		uint32_t data_;
};

using Strings = vector< string >;
using Tiles = vector< Tile >;

Tiles init_tiles()
{
	Tiles result;
	
	for ( auto &num : numbers )
	{
		for ( auto &col : colors )
		{
			result.push_back( { num, col } );
		}
	}
	
	auto seed = chrono::system_clock::now().time_since_epoch().count();

	shuffle( result.begin(), result.end(), default_random_engine( seed ) );
	
	return move( result );
}

struct Player
{
	string executable;
	Tiles inhand;
};

using Combinations = vector< Tiles >;

using Players = vector< Player >;

template < typename T >
T trim_left( T t )
{
	while ( !t.empty() && isspace( t.front() ) )
	{
		t.erase( t.begin() );
	}
	return forward< T >( t );
}

template < typename T >
T trim_right( T t )
{
	while ( !t.empty() && isspace( t.back() ) )
	{
		t.pop_back();
	}
	return forward< T >( t );
}

template < typename T >
T trim( T t )
{
	return trim_right( trim_left( t ) );
}

ostream& operator << ( ostream &str, const Tile &t )
{
	str << t.color();
	str << setw( 2 );
	str << setfill( '0' ) << static_cast< int >( t.number() );
	return str;
}

istream& operator >> ( istream &str, Tile &tile )
{
	char c = 0;
	int t = 0;
	str >> c;
	switch ( c )
	{
		case 'A': case 'B': case 'C': case 'D':
			break;
		default:
			return str;
	}
	str >> t;
	if ( t > 0 && t < 14 )
	{
		tile = Tile{
			static_cast< number::type >( 1 << ( t + 3 ) ),
			static_cast< color::type >( 1 << ( c - 'A' ) )
		};
	}
	return str;
}

istream& operator >> ( istream &str, Tiles &t )
{
	while ( str )
	{
		Tile tile;
		str >> tile;
		if ( tile.valid() )
		{
			t.push_back( move( tile ) );
		}
	}
	return str;
}

istream& operator >> ( istream &str, Combinations &combinations )
{
	for ( string line; getline( str, line ); )
	{
		Tiles tmp;
		istringstream( line ) >> tmp;
		if ( !tmp.empty() )
		{
			combinations.push_back( move( tmp ) );
		}
	}
	return str;
}

ostream& operator << ( ostream &str, const Tiles &t )
{
	copy( t.begin(), t.end(), ostream_iterator< Tile >( str, " " ) );
	return str;
}

ostream& operator << ( ostream &str, const Combinations &t )
{
	for ( auto &i : t )
	{
		str << i << '\n';
	}
	return str;
}

template < typename T >
string to_string( const T &t )
{
	stringstream s;
	s << t;
	return s.str();
}

string callProcess( string exe )
{
	unique_ptr< FILE, function< decltype( pclose ) > > stream( popen( exe.c_str(), "r" ), pclose );
	
	const auto fd = fileno( stream.get() );
	fcntl( fd, F_SETFL, O_NONBLOCK );
	
	string buffer;
	size_t total = 0;

	auto start = chrono::system_clock::now();
	size_t amount = 0;
	do
	{
		static const auto blocksize = 64;
		buffer.resize( total + blocksize );
		
		amount = read( fd, &buffer[ total ], buffer.size() );
		if ( amount == -1 )
		{
			if ( errno != EAGAIN )
			{
				throw runtime_error( "problem reading data from " + exe );
			}
			auto now = chrono::system_clock::now();
			if ( chrono::duration_cast< chrono::milliseconds >( now - start ).count() > 10000 )
			{
				throw runtime_error( exe + " took too long to respond!" );
			}
		}
		else if ( amount > 0 )
		{
			total += amount;
		}
		
		buffer.resize( total );
	}
	while ( amount );
	
	return move( buffer );
}

void validate( const string &result, const Player &player, const Combinations &field, const Tiles &pool )
{
	if ( trim( result ) == "draw" && pool.empty() )
	{
		throw runtime_error( "player " + player.executable + " tried to draw when there were no tiles left" );
	}
	
	Combinations check;
	istringstream( result ) >> check;
	
}

Tiles diff( Tiles a, Tiles b )
{
	sort( a.begin(), a.end() );
	sort( b.begin(), b.end() );
	Tiles result( a.size() + b.size() );
	
	auto it = set_difference( a.begin(), a.end(), b.begin(), b.end(), result.begin() );
	
	result.resize( it - result.begin() );
		
	return move( result );
}

Tiles diff( const Combinations &a, const Combinations &b )
{
	Tiles aa, bb;
	for ( auto &i : a )
	{
		aa.insert( aa.end(), i.begin(), i.end() );
	}
	for ( auto &i : b )
	{
		bb.insert( bb.end(), i.begin(), i.end() );
	}
	return diff( aa, bb );
}

bool setIsValid( Tiles tiles )
{
	auto begin = tiles.begin(), end = tiles.end();
	if ( end - begin < 3 )
	{
		return false;
	}

	sort( begin, end );
	
	Tile compare = *begin++;
	
	bool suit = true;
	bool straight = true;
	
	for ( ;begin != end && ( straight || suit ); ++begin )
	{
		if ( compare.color() != begin->color() )
		{
			suit = false;
		}
		
		if ( begin->number() - compare.number() != 1 )
		{
			straight = false;
		}
	}
	
	return straight || suit;
}

void checkCombinations( const Combinations &combinations )
{
	for ( auto &c : combinations )
	{
		if ( !setIsValid( c ) )
		{
			throw runtime_error( "invalid set: " + to_string( c ) );
		}
	}
}

size_t tileCount( const Combinations &combinations )
{
	size_t total = 0;
	for ( auto &set : combinations )
	{
		total += set.size();
	}
	return total;
}

void run_move( Player &player, Tiles &pool, Combinations &combinations )
{
	const string temporaryFileName( "/tmp/" + to_string( 1 ) + ".txt" );
	{
		ofstream temporaryFile( temporaryFileName );
		temporaryFile
			<< "hand\n"
			<< player.inhand
			<< "\nfield\n"
			<< combinations;
	}
	
	string result = callProcess( player.executable + " < " + temporaryFileName );

	Combinations check;
	istringstream( result ) >> check;
	
	if ( tileCount( check ) < tileCount( combinations ) )
	{
		throw runtime_error( "player " + player.executable + " removed tiles from field" );
	}
	
	auto difference = diff( check, combinations );
	
	// check for duplicates
	
	if ( difference.empty() )
	{
		if ( !pool.empty() )
		{
			player.inhand.push_back( move( pool.back() ) );
			pool.pop_back();
		}
	}
	else
	{
		for ( auto i : difference )
		{
			auto found = find( player.inhand.begin(), player.inhand.end(), i );
			
			if ( found == player.inhand.end() )
			{
				throw runtime_error( "player " + player.executable + " tried to place " + to_string( i ) + " which is not in his possesion" );
			}
			player.inhand.erase( found );
		}
		
		checkCombinations( check );
		
		combinations = check;
	}
	
	cout << "field:\n" << combinations << endl;
}

template < typename T >
void run_game( const T &&executables )
{
	Tiles pool = init_tiles();
	
	Players players;
	Combinations field;
	
	for ( auto &exe : executables )
	{
		Player p;
		p.executable = exe;
		for ( int i = 0; i < 13*4 && !pool.empty(); ++i )
		{
			p.inhand.push_back( move( pool.back() ) );
			pool.pop_back();
		}
		players.push_back( p );
	}
	
	size_t fieldSize = -1;
	int round = 1;
	while ( pool.size() || fieldSize != field.size() )
	{
		fieldSize = field.size();
		
		cout << "round: " << round++ << endl;
		
		for ( auto &p : players )
		{
			run_move( p, pool, field );
			if ( p.inhand.empty() )
			{
				cout << "player " + p.executable + " won!" << endl;
				return;
			}
		}
	}
	
	cout << "remise!" << endl;
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