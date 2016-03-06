#ifndef FILE_BUFFER_C_H
#define FILE_BUFFER_C_H

#include <n32_base.h>

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

template <class T>
class file_buffer_c
{

private:
	uAL32Bits	m_uBufferSize;
	uAL32Bits	m_uBufferCapacity;
	T *		m_pDataStart;

	// file_buffer_c(const file_buffer_c& rhs); since is it stored in STL
	// file_buffer_c& operator=(const file_buffer_c& rhs); since is it stored in STL

public:
	file_buffer_c( uAL32Bits i_uBufferCapacity= 0x0 ):m_uBufferSize(0),m_uBufferCapacity(i_uBufferCapacity),m_pDataStart(0)
	{
		Init( m_uBufferCapacity );
	};
	
	T * getBufferAddr( void ) const
	{
		return( m_pDataStart );
	}
	
	uAL32Bits getBufferSize( void ) const
	{
		return( m_uBufferSize );
	}
	
	result_t addBufferSize( uAL32Bits i_uElementsToAdd )
	{
		if ( likely( ( m_uBufferSize + i_uElementsToAdd ) <= m_uBufferCapacity ) )
		{
			m_uBufferSize += i_uElementsToAdd;
			
			return( N32_NO_ERROR );
		}
		else
		{
			return( N32_ERR_BAD_ALGORITHM );	
		}
	}
	
	result_t AppendStringFromOffset( uAL32Bits i_uElementsToSkip, const string & i_DataToBeAppended ) 
	{
		if ( likely( ( i_uElementsToSkip + i_DataToBeAppended.size() ) <= m_uBufferCapacity ) )
		{
			memcpy( m_pDataStart + i_uElementsToSkip, &*i_DataToBeAppended.begin(), i_DataToBeAppended.size() );
			
			m_uBufferSize += i_DataToBeAppended.size();
			
			return( N32_NO_ERROR );
		}
		else
		{
			return( N32_ERR_BAD_ALGORITHM );	
		}
	}
	
	result_t Init( uAL32Bits i_uBufferCapacity )
	{
		if ( likely( 0x0 != i_uBufferCapacity ) )
		{
			m_pDataStart= new T [ i_uBufferCapacity ];
			if ( unlikely( N32_NULL == m_pDataStart ) )
			{
				return( N32_ERR_MEM );	
			}
			
			m_uBufferCapacity= i_uBufferCapacity;
			return( N32_NO_ERROR );	
		}
		else
		{
			return( N32_ERR_BAD_ALGORITHM );
		}
	}

	result_t ReadFileIntoBuffer( const string & i_FromFileName )
	{
		ifstream file( i_FromFileName.c_str(), ios::in|ios::binary|ios::ate );
		if ( likely( file.is_open() ) )
		{
			m_uBufferSize= file.tellg();

			if ( unlikely( m_uBufferCapacity < m_uBufferSize ) )
			{
				m_uBufferCapacity= m_uBufferSize;

				m_pDataStart= new T [ m_uBufferCapacity ];
				if ( unlikely( N32_NULL == m_pDataStart ) )
				{
					file.close();
					DEB( DEB_WARN, "Err: 0x%x\n", N32_ERR_BAD_ALGORITHM );
					return( N32_ERR_MEM );
				}
			}

			file.seekg( 0, ios::beg );
			file.read( m_pDataStart, m_uBufferSize );
			file.close();

			return( N32_NO_ERROR );
		}

		return( N32_ERR_UNKNOWN );
	}

	result_t WriteFileFromBuffer( const string & i_ToFileName )
	{
		ofstream file( i_ToFileName.c_str(), ios::out|ios::binary );

		if ( likely( file.is_open() ) )
		{
			file.write( m_pDataStart, m_uBufferSize );
			file.close();

			return( N32_NO_ERROR );
		}

		return( N32_ERR_UNKNOWN );
	}

	virtual ~file_buffer_c()
	{
		if ( likely( N32_NULL != m_pDataStart ) )
		{
			delete [] m_pDataStart;
		}
	}

};

#endif // FILE_BUFFER_C_H
