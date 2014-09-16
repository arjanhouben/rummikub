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

struct Option
{
	Combinations sets;
	Tiles inhand;
	Tiles tiles() const
	{
		Tiles total { inhand };
		for ( auto &s : sets )
		{
			total.insert( total.end(), s.begin(), s.end() );
		}
		return total;
	}
};

using Options = vector< Option >;

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

using filter_type = function< bool(value_type) >;

template < typename Container >
Container operator | ( Container container, filter_type f )
{
	container.erase( remove_if( container.begin(), container.end(), f ), container.end() );
	return container;
}

filter_type keep( value_type t )
{
	return { [t]( value_type i ) { return ( i & t ) == 0; } };
}

filter_type remove( value_type t )
{
	return { [t]( value_type i ) { return ( i & t ) != 0; } };
}

template < typename T >
T unique( T source )
{
	source.erase( unique( source.begin(), source.end() ), source.end() );
	return source;
}

value_type mask( const Tiles &tiles )
{
	value_type m = 0;
	for ( auto &t : tiles ) m |= t;
	return m;
}

void removeFromTiles( Tiles &tiles, const Tiles remove )
{
	for ( auto &r : remove )
	{
		for ( auto i = tiles.begin(); i != tiles.end(); ++i )
		{
			if ( *i == r )
			{
				tiles.erase( i );
				break;
			}
		}
	}
}

void appendToField( Combinations &field, Tiles &hand )
{
	for ( auto &set : field )
	{
		const auto m = mask( set );
		const auto colorMask = static_cast< color::type >( m & color::mask );
		const auto numberMask = static_cast< number::type >( m & number::mask );
		switch ( colorMask )
		{
			case color::red:
			case color::yellow:
			case color::blue:
			case color::black:
			{
				auto required = Tile( static_cast< number::type >( set.front().number() - 1 ), colorMask );
				auto found = find( hand.begin(), hand.end(), required );
				if ( found != hand.end() )
				{
					set.insert( set.begin(), *found );
					hand.erase( found );
				}
				required = Tile( static_cast< number::type >( set.back().number() + 1 ), colorMask );
				found = find( hand.begin(), hand.end(), required );
				if ( found != hand.end() )
				{
					set.push_back( *found );
					hand.erase( found );
				}
			}
			default:
				break;
		}
		switch ( numberMask )
		{
			case number::one:
			case number::two:
			case number::three:
			case number::four:
			case number::five:
			case number::six:
			case number::seven:
			case number::eight:
			case number::nine:
			case number::ten:
			case number::eleven:
			case number::twelve:
			{
				auto add = unique( hand | keep( numberMask ) | remove( colorMask ) );
				set.insert( set.end(), add.begin(), add.end() );
				sort( set.begin(), set.end() );
				removeFromTiles( hand, add );
			}
			default:
				break;
		}
	}
}

Tiles findNumberSequence( Tiles &hand )
{
	return {};
}

int main(int,char**)
{
	try
	{
		istream &input( cin );
		
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
		
		appendToField( field, hand );
		
		for ( auto found = findNumberSequence( hand ); !found.empty(); )
		{
			field.push_back( found );
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
