#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iterator>
#include <fstream>
#include <functional>
#include <algorithm>
#include <iomanip>

using namespace std;

using value_type = uint32_t;

class test
{
	public:
		template < typename T >
		test( T &&t )
		{
			tests_.push_back( move( t ) );
		}
		static void run()
		{
			for ( auto &test : tests_ )
			{
				test();
			}
		}
		template < typename A, typename B >
		static void equal( size_t lineNr, A a, B b )
		{
			if ( a != b )
			{
				cerr << "error on line " + to_string( lineNr ) << ", " << a << " not equal to " << b << endl;
				exit( 1 );
			}
		}
		static void is_true( size_t lineNr, bool a )
		{
			if ( !a )
			{
				cerr << "error on line " + to_string( lineNr ) << endl;
				exit( 1 );
			}
		}
	private:
		static vector< function< void() > > tests_;
};

vector< function< void() > > test::tests_;

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
		
		Tile( color::type n, number::type c ) :
			data_( n | c ) {}
	
		Tile() :
			data_( 0 ) {}
		
		operator value_type() const
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
		value_type data_;
};

using Strings = vector< string >;
using Tiles = vector< Tile >;

Tiles operator + ( Tiles a, const Tiles &b )
{
	a.insert( a.end(), b.begin(), b.end() );
	return a;
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

struct Combinations : vector< Tiles >
{
	Tiles tiles() const
	{
		Tiles result;
		for ( auto &set : *this )
		{
			result.insert( result.end(), set.begin(), set.end() );
		}
		return result;
	}
};

template < typename T >
istream& operator >> ( istream &str, T &tiles )
{
	typename T::value_type t;
	while ( str >> t )
	{
		tiles.push_back( t );
	}
	return str;
}

ostream& operator << ( ostream &stream, const Tiles &t )
{
	auto dst = ostream_iterator< Tiles::value_type >( stream, " " );
	copy( begin( t ), end( t ), dst );
	return stream;
}

ostream& operator << ( ostream &stream, const Combinations &t )
{
	auto dst = ostream_iterator< Combinations::value_type >( stream, "\n" );
	copy( begin( t ), end( t ), dst );
	return stream;
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

inline value_type hamming_weight( value_type n )
{
	n = n - ((n>>1) & 0x55555555);
	n = (n & 0x33333333) + ((n>>2) & 0x33333333);
	return (((n + (n>>4)) & 0xF0F0F0F) * 0x1010101) >> 24;
}

inline bool adjecent( value_type a, value_type b )
{
	return ( ( a >> 1 ) | ( a << 1 ) ) & b;
}

test adjecent_test = []()
{
	value_type a, b;
	a = 1 << 2;
	b = 1 << 3;
	test::is_true( __LINE__, adjecent( a, b ) );
	a = 1 << 4;
	test::is_true( __LINE__, adjecent( a, b ) );
	a = 1 << 5;
	test::is_true( __LINE__, !adjecent( a, b ) );
};


using filter_type = function< bool(value_type) >;

template < typename Container >
Container operator | ( const Container &container, filter_type f )
{
	Container result;
	for ( auto &i : container )
	{
		if ( f( i ) )
		{
			result.push_back( i );
		}
	}
	return result;
}

filter_type filter( value_type t )
{
	return { [t]( value_type i ) { return i & t; } };
}

template < typename T >
void unique( T &source )
{
	auto end = unique( source.begin(), source.end() );
	source.resize( end - source.begin() );
}

void remove( Tiles &tiles, const Tile &tile )
{
	auto found = find( tiles.begin(), tiles.end(), tile );
	if ( found != tiles.end() )
	{
		tiles.erase( found );
	}
}

void remove( Tiles &tiles, const Tiles &removetiles )
{
	for ( auto &r : removetiles )
	{
		remove( tiles, r );
	}
}

void remove( Tiles &tiles, const Combinations &combinations )
{
	for ( auto &set : combinations )
	{
		for ( auto &tile : set )
		{
			remove( tiles, tile );
		}
	}
}

Combinations createCombinations( Tiles tiles )
{
	Combinations result;
	
	for ( auto &col : { color::red, color::yellow, color::blue, color::black } )
	{
		auto colortiles = tiles | filter( col );
		sort( colortiles );
		unique( colortiles );
		auto start = colortiles.begin();
		auto val = ( *start++ ) & number::mask;
		while ( start != colortiles.end() )
		{
			auto nextval = *start & number::mask;
			if ( !adjecent( val, nextval ) )
			{
				break;
			}
			
			val = nextval;
			
			++start;
		}
		auto size = start - colortiles.begin();
		if ( size > 2 )
		{
			result.push_back( { colortiles.begin(), start } );
			
			remove( tiles, result.back() );
		}
	}
	
	for ( auto &num : { number::one, number::two, number::three, number::four, number::five, number::six, number::seven, number::eight, number::nine, number::ten, number::eleven, number::twelve, number::thirteen } )
	{
		auto numbertiles = tiles | filter( num );
		sort( numbertiles );
		unique( numbertiles );
		auto mask = color::mask;
		Tiles tmp;
		for ( auto &t : numbertiles )
		{
			if ( t & mask )
			{
				tmp.push_back( t );
			}
		}
		if ( tmp.size() > 2 )
		{
			remove( tiles, tmp );
			result.push_back( move( tmp ) );
		}
	}
	
	return result;
}

int main(int,char**)
{
	try
	{
		test::run();
		
		ifstream input( "/tmp/1.txt" );
		//	istream &input( cin );
		
		string line;
		while ( getline( input, line ) )
		{
			if ( line == "hand" )
			{
				getline( input, line );
				break;
			}
		}
		Tiles hand;
		istringstream( line ) >> hand;
		while ( getline( input, line ) )
		{
			if ( line == "field" )
			{
				break;
			}
		}
		
		Combinations field;
		while ( getline( input, line ) )
		{
			Tiles tiles;
			istringstream( line ) >> tiles;
			if ( !tiles.empty() )
			{
				field.push_back( tiles );
			}
		}
		
		Tiles combined = field.tiles() + hand;
		
		field.clear();

		Combinations fromHand = createCombinations( combined );
		for ( auto &s : fromHand )
		{
			field.push_back( s );
		}
		
		cout << field << endl;
	}
	catch ( const exception &err )
	{
		cerr << err.what() << endl;
		return 1;
	}
	
	return 0;
}
