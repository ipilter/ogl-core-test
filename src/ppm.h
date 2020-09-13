#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>

class ppm
{
public:
    struct pixel
    {
        typedef unsigned char channel_t;
        channel_t  mRed, mGreen, mBlue;

        pixel( const channel_t r = 0, const channel_t g = 0, const channel_t b = 0 )
         : mRed( r ), mGreen( g ), mBlue( b )
        { }

        static channel_t maxColor()
        {
            return 255;
        }

        pixel& operator += (const pixel& rhs)
        {
          mRed += rhs.mRed;
          mGreen += rhs.mGreen;
          mBlue += rhs.mBlue;
          return *this;
        }
        pixel operator + (const pixel& rhs)
        {
          return pixel(mRed + rhs.mRed
                       , mGreen + rhs.mGreen
                       , mBlue + rhs.mBlue);
        }
        pixel& operator -= (const pixel& rhs)
        {
          mRed -= rhs.mRed;
          mGreen -= rhs.mGreen;
          mBlue -= rhs.mBlue;
          return *this;
        }
        pixel operator - (const pixel& rhs)
        {
          return pixel(mRed - rhs.mRed
                       , mGreen - rhs.mGreen
                       , mBlue - rhs.mBlue);
        }
    };
    typedef std::vector<pixel> pixel_buffer;
    typedef pixel_buffer::size_type size_t;

public:
    ppm(const uvec2& size, const pixel& color = pixel())
      : m_size(size)
      , m_data(m_size.x * m_size.y, color)
    { }
public:
    /*
     * Sets the pixel at the given coordinates
     */
    void set_pixel( const uvec2& pos, const pixel& p )
    {
      m_data[pos.x + pos.y * m_size.x] = p;
    }

    pixel get_pixel( const uvec2& pos) const
    {
        return m_data[pos.x + pos.y * m_size.x];
    }
public:
    const uvec2& size() const
    {
        return m_size;
    }

    size_t byte_count() const
    {
        return m_data.size() * sizeof(pixel);
    }

    pixel* memory()
    {
      return &m_data[0];
    }
public:
    /*
     * Writes the current image to the given file path
     */
    void write( const std::string& file ) const
    {
        std::ofstream stream( file.c_str(), std::ios::binary | std::ios::out );
        if( !stream.is_open() )
            throw std::runtime_error( std::string( "could not open ppm file: " ) + file );
        stream << "P6" << std::endl << m_size.x << " " << m_size.y << std::endl << (int)pixel::maxColor() << std::endl;
        stream.write( reinterpret_cast<const char*>( &m_data[0] ), byte_count());
    }
private:
    const uvec2 m_size;
    pixel_buffer m_data;
};
