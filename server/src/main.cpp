#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <memory>
#include <fstream>
#include <iterator>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <stdexcept>

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

size_t value( number::type t )
{
	switch ( t )
	{
		case number::one:
			return 1;
		case number::two:
			return 2;
		case number::three:
			return 3;
		case number::four:
			return 4;
		case number::five:
			return 5;
		case number::six:
			return 6;
		case number::seven:
			return 7;
		case number::eight:
			return 8;
		case number::nine:
			return 9;
		case number::ten:
			return 10;
		case number::eleven:
			return 11;
		case number::twelve:
			return 12;
		case number::thirteen:
			return 13;
		default:
			return 0;
	}
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
	
	operator uint32_t() const
	{
		return data_;
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
	
	for ( int i = 0; i < 2; ++i )
	{
		for ( auto &num : numbers )
		{
			for ( auto &col : colors )
			{
				result.push_back( { num, col } );
			}
		}
	}
	
	auto seed = chrono::system_clock::now().time_since_epoch().count();

	shuffle( begin( result ), end( result ), default_random_engine( /*seed*/0 ) );
	
	return result;
}

struct Player
{
	string executable;
	bool disqualified;
	Tiles inhand;
	int id;
	
	string name() const
	{
		return executable.substr( executable.rfind( '/' ) + 1 ) + '(' + to_string( id ) + ')';
	}
};

using Combinations = vector< Tiles >;

using Players = vector< Player >;

template < typename T >
T trim_left( T t )
{
	while ( !t.empty() && isspace( t.front() ) )
	{
		t.erase( begin( t )() );
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

ostream& operator << ( ostream &stream, const Tile &t )
{
	switch ( t.color() )
	{
		case color::red:
			stream << 'A';
			break;
		case color::yellow:
			stream << 'B';
			break;
		case color::blue:
			stream << 'C';
			break;
		case color::black:
			stream << 'D';
			break;
		default:
			throw runtime_error( "unknown color" );
	}
	
	stream << setw( 2 ) << setfill( '0' ) << value( t.number() );
	
	return stream;
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
	copy( begin( t ), end( t ), ostream_iterator< Tile >( str, " " ) );
	return str;
}

ostream& operator << ( ostream &str, const Combinations &t )
{
	copy( begin( t ), end( t ), ostream_iterator< Combinations::value_type >( str, "\n" ) );
	return str;
}

template < typename T, typename Cmp >
void sort( T &t, Cmp c )
{
	sort( begin( t ), end( t ), c );
}

template < typename T >
void sort( T &t )
{
	sort( begin( t ), end( t ) );
}

template < typename T >
string to_string( const T &t )
{
	stringstream s;
	s << t;
	return s.str();
}

string callProcess( const string &exe )
{
	unique_ptr< FILE, decltype( &pclose ) > stream( popen( exe.c_str(), "r" ), &pclose );
	
	const auto fd = fileno( stream.get() );
	fcntl( fd, F_SETFL, O_NONBLOCK );
	
	string buffer;
	size_t total = 0, amount = 0;

	const auto start = chrono::system_clock::now();
	do
	{
		static const auto blocksize = 64;
		buffer.resize( total + blocksize );
		
		amount = read( fd, &buffer[ total ], buffer.size() );
		if ( amount == -1 )
		{
			if ( errno != EAGAIN )
			{
				throw runtime_error( "problem reading data" );
			}
			
			auto now = chrono::system_clock::now();
			if ( chrono::duration_cast< chrono::milliseconds >( now - start ).count() > 10000 )
			{
				throw runtime_error( "took too long to respond" );
			}
			
			usleep( 10 );
		}
		else if ( amount > 0 )
		{
			total += amount;
		}
		
		buffer.resize( total );
	}
	while ( amount );
	
	return buffer;
}

Tiles diff( Tiles a, Tiles b )
{
	sort( begin( a ), end( a ) );
	sort( begin( b ), end( b ) );
	Tiles result( a.size() + b.size() );
	
	auto it = set_difference( begin( a ), end( a ), begin( b ), end( b ), begin( result ) );
	
	result.resize( it - begin( result ) );
		
	return result;
}

Tiles diff( const Combinations &a, const Combinations &b )
{
	Tiles aa, bb;
	for ( auto &i : a )
	{
		aa.insert( end( aa ), begin( i ), end( i ) );
	}
	for ( auto &i : b )
	{
		bb.insert( end( bb ), begin( i ), end( i ) );
	}
	return diff( aa, bb );
}

inline uint32_t hamming_weight( uint32_t n )
{
	n = n - ((n>>1) & 0x55555555);
	n = (n & 0x33333333) + ((n>>2) & 0x33333333);
	return ((n + (n>>4) & 0xF0F0F0F) * 0x1010101) >> 24;
}

inline bool sequential( uint32_t v, uint32_t c )
{
	if ( !v || hamming_weight( v ) != c )
	{
		return false;
	}
	
	// move till first set bit
	while ( !(v & 1) )
	{
		v >>= 1;
	}
	
	// count set bits
	while ( v & 1 )
	{
		v >>= 1;
		--c;
	}
	
	// all bits should have been tested, so v == 0
	return ( c == 0 ) && ( v == 0 );
}

bool setIsValid( const Tiles &tiles )
{
	auto begin = tiles.begin(), end = tiles.end();
	const auto size = end - begin;
	if ( size < 3 )
	{
		return false;
	}

	uint32_t mask = 0;
	
	while ( begin != end )
	{
		mask |= *begin++;
	}
	
	switch ( mask & color::mask )
	{
		case color::red:
		case color::yellow:
		case color::blue:
		case color::black:
			return sequential( mask & number::mask, size );
			
		case color::red | color::yellow | color::blue: // no black
		case color::red | color::yellow | color::black: // no blue
		case color::red | color::black | color::blue: // no yellow
		case color::yellow | color::black | color::blue: // no red
			return size == 3 && hamming_weight( mask & number::mask ) == 1;
			
		case color::mask:
			return size == 4 && hamming_weight( mask & number::mask ) == 1;
			
		default:
			return false;
	}
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

template < typename S >
void generatePlayerInput( Player &player, const Combinations &combinations, S &&stream )
{
	sort( player.inhand.begin(), player.inhand.end() );
	stream
		<< "hand\n"
		<< player.inhand
		<< "\nfield\n"
		<< combinations;
}

void run_move( Player &player, Tiles &pool, Combinations &combinations )
{
	const string temporaryFileName( "/tmp/" + to_string( 1 ) + ".txt" );

	generatePlayerInput( player, combinations, ofstream( temporaryFileName ) );
	
	const string result = callProcess( player.executable + " < " + temporaryFileName );
	
	cout << result;

	Combinations check;
	istringstream( result ) >> check;
		
	if ( tileCount( check ) < tileCount( combinations ) )
	{
		throw runtime_error( "tiles removed from field" );
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
				throw runtime_error( to_string( i ) + " was not owned" );
			}
			player.inhand.erase( found );
		}
		
		checkCombinations( check );
		
		combinations = check;
	}
}

size_t points( const Tiles &tiles )
{
	size_t total = 0;
	for ( auto &i : tiles )
	{
		total += value( i.number() );
	}
	return total;
}

void endGame( Tiles &pool, Combinations &field, Players &players )
{
	sort( players,
		[]( const Player &a, const Player &b )
		{
			if ( a.disqualified == b.disqualified )
			{
				return points( a.inhand ) < points( b.inhand );
			}
			
			return !a.disqualified;
		}
	);
	
	size_t position = 0;
	cout << "game is finished, score:\n";
	for ( auto &p : players )
	{
		cout << "nr " << ++position << ": " << p.name();
		if ( p.disqualified )
		{
			cout << "(disqualified)";
		}
		cout << ", " << to_string( points( p.inhand ) ) << " [ " << p.inhand << " ]" << '\n';
	}
}

template < typename T >
Players getPlayers( T &&executables, Tiles &pool )
{
	Players players;
	
	int id = 0;
	for ( auto &exe : executables )
	{
		Player p;
		p.id = ++id;
		p.executable = exe;
		auto start = pool.begin();
		auto end = start + min< size_t >( 16, pool.size() );
		p.inhand.assign( start, end );
		pool.erase( start, end );
		players.push_back( p );
	}
	
	return players;
}

void run_game( Tiles &pool, Players &players, Combinations &field )
{
	size_t fieldSize = -1;
	int round = 1;
	while ( pool.size() || fieldSize != field.size() )
	{
		fieldSize = field.size();
		
		cout << "round: " << round++ << endl;
		
		for ( auto &p : players )
		{
			cout << "player: " << p.name() << "\n"
				<< ">>>\n";
			generatePlayerInput( p, field, cout );
			cout << "\n<<<\n";
			
			try
			{
				run_move( p, pool, field );
			}
			catch ( ... )
			{
				p.disqualified = true;
				throw;
			}
			
			if ( p.inhand.empty() )
			{
				return;
			}
		}
	}
}

int main( int argc, char *argv[] )
{
	Tiles pool;
	Players players;
	Combinations field;
	
	try
	{
		if ( argc < 2 )
		{
			throw runtime_error( "no clients specified" );
		}
		
		pool = init_tiles();
		players = getPlayers( Strings( argv + 1, argv + argc ), pool );
		
		run_game( pool, players, field );
	}
	catch ( const exception &err )
	{
		cerr << err.what() << endl;
		
		endGame( pool, field, players );
		
		return 1;
	}
	
	endGame( pool, field, players );
	
	return 0;
}
