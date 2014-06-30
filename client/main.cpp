#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iterator>
#include <fstream>

using namespace std;

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
	
		uint32_t operator | ( const Tile &rhs ) const
		{
			return data_ | rhs.data_;
		}
		
		uint32_t operator & ( const Tile &rhs ) const
		{
			return data_ & rhs.data_;
		}
		
		uint32_t operator | ( uint32_t rhs ) const
		{
			return data_ | rhs;
		}
		
		uint32_t operator & ( uint32_t rhs ) const
		{
			return data_ & rhs;
		}
		
	private:
		uint32_t data_;
};

using Strings = vector< string >;
using Tiles = vector< Tile >;

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
	
	switch ( t.number() )
	{
		case number::one:
			stream << "01";
			break;
		case number::two:
			stream << "02";
			break;
		case number::three:
			stream << "03";
			break;
		case number::four:
			stream << "04";
			break;
		case number::five:
			stream << "05";
			break;
		case number::six:
			stream << "06";
			break;
		case number::seven:
			stream << "07";
			break;
		case number::eight:
			stream << "08";
			break;
		case number::nine:
			stream << "09";
			break;
		case number::ten:
			stream << "10";
			break;
		case number::eleven:
			stream << "11";
			break;
		case number::twelve:
			stream << "12";
			break;
		case number::thirteen:
			stream << "13";
			break;
		default:
			throw runtime_error( "unknown number" );
	}
	
	return stream;
}

using Combinations = vector< Tiles >;

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

template < typename T >
void sort( T &t )
{
	sort( begin( t ), end( t ) );
}

inline uint32_t hamming_weight( uint32_t n )
{
	n = n - ((n>>1) & 0x55555555);
	n = (n & 0x33333333) + ((n>>2) & 0x33333333);
	return ((n + (n>>4) & 0xF0F0F0F) * 0x1010101) >> 24;
}

inline bool adjecent( uint32_t a, uint32_t b )
{
	return ( ( a >> 1 ) | ( a << 1 ) ) & b;
}

test adjecent_test = []()
{
	uint32_t a, b;
	a = 1 << 2;
	b = 1 << 3;
	test::is_true( __LINE__, adjecent( a, b ) );
	a = 1 << 4;
	test::is_true( __LINE__, adjecent( a, b ) );
	a = 1 << 5;
	test::is_true( __LINE__, !adjecent( a, b ) );
};

Tiles::iterator find_next_in_sequence( Tile tile, Tiles &pool )
{
	auto next = [&]( const Tile &t )
	{
		return ( t.color() == tile.color() ) && ( adjecent( tile.number(), t.number() ) );
	};
	return find_if( pool.begin(), pool.end(), next );
}

Tiles::iterator find_next_in_color( const Tiles &seq, Tiles &pool )
{
//	auto next = [&]( const Tile &t )
//	{
//		return ( t.number() == tile.number() ) && ( tile.color() != t.color() );
//	};
//	return find_if( pool.begin(), pool.end(), next );
}

Tiles find_next( Tile tile, Tiles &pool )
{
	Tiles tmp_color = pool;
	Tiles tmp_number = pool;
	
	Tiles colorseq { tile };
	for ( auto result = find_next_in_color( colorseq, tmp_color ); result != tmp_color.end(); )
	{
		colorseq.push_back( *result );
		tmp_color.erase( result );
		result = find_next_in_color( colorseq, tmp_color );
	}
	
	Tiles numberseq { tile };
	for ( auto result = find_next_in_sequence( numberseq.back(), tmp_number ); result != tmp_number.end(); )
	{
		numberseq.push_back( *result );
		tmp_number.erase( result );
		result = find_next_in_sequence( numberseq.back(), tmp_number );
	}
	
	if ( colorseq.size() > numberseq.size() )
	{
		pool = tmp_color;
		return colorseq;
	}
	
	pool = tmp_number;
	return numberseq;
}

test find_next_test = []()
{
	Tile q = { color::red, number::one };
	Tiles expected = {
		{ color::red, number::one },
		{ color::red, number::two },
		{ color::red, number::three }
	};
	Tiles input = {
		{ color::red, number::two },
		{ color::red, number::three },
		{ color::red, number::one }
	};
	test::equal( __LINE__, expected, find_next( expected.front(), input ) );
};

using filter_type = function< bool(uint32_t) >;

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

filter_type filter( uint32_t t )
{
	return { [t]( uint32_t i ) { return i & t; } };
}

template < typename T >
void unique( T &source )
{
	auto end = unique( source.begin(), source.end() );
	source.resize( end - source.begin() );
}

Combinations createCombinations( Tiles tiles )
{
	sort( tiles );
	
	Combinations result;
	
	for ( auto &col : { color::red, color::yellow, color::blue, color::black } )
	{
		auto colortiles = tiles | filter( col );
		sort( colortiles );
		unique( colortiles );
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
		
		Combinations fromHand = createCombinations( hand );
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
