/**
 * Copyright (c) 2015-2018 GMRV/URJC.
 *
 * Authors: Juan Jose Garcia Cantero <juanjose.garcia@urjc.es>
 *
 * This file is part of neurolots <https://github.com/gmrvvis/neurolots>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include "VDMapReader.h"

//OpenGL
#ifndef NEUROLOTS_SKIP_GLEW_INCLUDE
#include <GL/glew.h>
#endif
#ifdef Darwin
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifdef NEUROLOTS_USE_TIFF
#include <tiffio.h>
#endif

#ifdef NEUROLOTS_USE_QT5CORE
#include <QStringList>
#include <QString>
#include <QXmlStreamReader>
#include <QFile>
#include <QFileInfo>
#endif

#include <boost/filesystem.hpp>

namespace nlgeometry
{
  VDMapPtr VDMapReader::readVDMap( const std::string& vdmFile_,
                                   const std::string& normalFile_ )
  {
    unsigned int vdmSize;
    unsigned int normalSize;
    auto vdmTexture = _readTexture( vdmFile_, vdmSize );
    auto normalTexture = _readTexture( normalFile_, normalSize );

    if ( !vdmTexture || !normalTexture || vdmSize != normalSize )
    {
      if ( vdmTexture )
        delete vdmTexture;
      if ( normalTexture )
        delete normalTexture;
      return nullptr;
    }
    auto vdmap = new VDMap( );
    vdmap->vdmTexture( vdmTexture );
    vdmap->normalTexture( normalTexture );
    vdmap->size( ) = normalSize;
    return vdmap;
  }

   std::vector <float> PCAReader::readPCATexture(const std::string& pcaFile, unsigned int& width_)
   {
    TIFF* tifFile = TIFFOpen( pcaFile.c_str( ), "r" );


    TIFFGetField( tifFile, TIFFTAG_IMAGEWIDTH, &width_ );

    const float height=1;


    unsigned int rowBytesSize = width_ * 3 * 4; //sizeof(int) is 4 bytes
    std::vector< float > pixels( width_ * height * 3 );
    unsigned char * buf = ( unsigned char* )_TIFFmalloc( rowBytesSize );

    //if ( TIFFReadScanline( tifFile, buf, 0 ) < 0 )
      //return nullptr;
    TIFFReadScanline( tifFile, buf, 0 );
    memcpy( &pixels[0], buf, rowBytesSize );

    _TIFFfree( buf );
    TIFFClose( tifFile );

    auto texConfig = reto::TextureConfig( );
    texConfig.internalFormat = GL_RGB32F;
    texConfig.format = GL_R32F; //por darle uno
    texConfig.type = GL_FLOAT;
    texConfig.wrapS = GL_CLAMP_TO_EDGE;
    texConfig.wrapT = GL_CLAMP_TO_EDGE;

    std::cout << "Espina -> rowBytesSize " << std::to_string(rowBytesSize) << std::endl ;
   /* int count=0;


    for( size_t i = 0 ; i < width_ *3 ; i++ )
    {
      count++;
       std::cout << pixels.data()[i] << " " ;
    }
    std::cout<< "Size :" << std::to_string(count) << std:: endl;*/

    //return new reto::Texture1D( texConfig, pixels.data( ), width_ );

    return pixels;
   }



  nlgeometry::PCACompMapPtr PCAReader::readMacrotextureComponents(const std::string* pcaComponentsPath, int& numComponentes)
  {
    unsigned int componentSize;
    std::vector<float> components;


    std::vector<float> componentTexture;
    for(int i=0; i<numComponentes;i++)
    {
        componentTexture = PCAReader::_readTexture( pcaComponentsPath[i], componentSize );

        std::cout << "component " << std::to_string(i)<< " size " << componentTexture.size() << std:: endl; 


        for (int j=0; j<componentTexture.size(); j++) 
        {
          components.push_back(componentTexture[j]); 
        }
    } 

    std::cout << "componentents size " << components.size() << std:: endl; 

    std::vector <void *> componentsVoid{components.data(), components.data() + components.size()};

    for (int i=0; i<componentTexture.size(); i++) 
    {
      componentsVoid.push_back(&componentTexture[i]); 
    }
    

    if ( numComponentes * componentSize != numComponentes *65) //65 es el width. es por tener algo con lo que saber que aqui existe algo
    {
      std::cout<<"algo s'ha roto" << std::endl;
       //componentTexture.clear();
      return nullptr;
    }

    auto texConfig = reto::TextureConfig();
    texConfig.internalFormat=GL_RGB32F;
    texConfig.format= GL_RGB;
    texConfig.type= GL_FLOAT;
    texConfig.wrapS= GL_CLAMP_TO_EDGE;
    texConfig.wrapT= GL_CLAMP_TO_EDGE;

    reto::Texture2DArray * componentMap= new reto::Texture2DArray( texConfig, componentsVoid, numComponentes, componentSize,componentSize );

    auto pcaComponentMap = new PCAComponentMap( ); 
    pcaComponentMap->textureComponents(componentMap);
    pcaComponentMap->size()= componentSize * componentSize * numComponentes * 3;

    return pcaComponentMap;
  }



  VDMapCollectionPtr VDMapReader::readVDMapCollection(
#ifdef NEUROLOTS_USE_QT5CORE
    const std::string& fileName_ )
  {
    std::string rootPath;
    boost::filesystem::path p( fileName_ );
    if ( !p.parent_path( ).empty( ))
      rootPath = p.parent_path( ).string( ) + "/";
    QFile qFile( fileName_.c_str( ));
    if ( !qFile.exists( ))
    {
      std::cerr << "Error: file: " << fileName_ << " not found." << std::endl;
      return nullptr;
    }

    qFile.open( QIODevice::ReadOnly | QIODevice::Text );
    if ( !qFile.isOpen( ))
    {
      std::cerr << "Error: file: " << fileName_ << " not readable."
                << std::endl;
      return nullptr;
    }

    QXmlStreamReader xml( & qFile );
    if ( xml.hasError( ) )
    {
      std::cerr << "Scene XML file: " << fileName_ << " has errors"
                << std::endl;
      return nullptr;
    }

    xml.readNextStartElement( );
    if ( xml.name( ) != "vdmaps" )
    {
      std::cerr << "Expected vdmaps root elemnt" << std::endl;
      return nullptr;
    }
    // bool collectionLoaded = false;

    VDMapCollectionPtr vdmCollec = new VDMapCollection( );
    QXmlStreamAttributes attributes = xml.attributes( );
    if ( attributes.hasAttribute( "vdmapsSize" ))
      vdmCollec->vdmapsSize( attributes.value( "vdmapsSize" ).toUInt( ));

    while( !xml.atEnd( ) &&  !xml.hasError( ))
    {
      xml.readNext( );
      if ( xml.atEnd( ) || xml.tokenType( ) != QXmlStreamReader::StartElement )
        continue;
      if ( xml.name( ) == "vdmap" )
      {
        attributes = xml.attributes( );
        if( attributes.hasAttribute( "vdmapTexture" ) &&
            attributes.hasAttribute("normalTexture"))
        {
          std::string vdmapTexture = rootPath;
          vdmapTexture.append(
            attributes.value( "vdmapTexture" ).toString( ).toStdString( ));
          std::string normalTexture = rootPath;
          normalTexture.append(
            attributes.value( "normalTexture" ).toString( ).toStdString( ));
          auto vdmap = readVDMap( vdmapTexture, normalTexture );
          if ( !vdmap )
            continue;
          Eigen::Matrix4f transform = Eigen::Matrix4f::Identity( );
          while( !xml.atEnd( ) &&  !xml.hasError( ) &&
                 !( xml.name( ) == "vdmap" &&
                    xml.tokenType( ) == QXmlStreamReader::EndElement ))
          {
            xml.readNext( );
            if ( xml.atEnd( ) || xml.tokenType( ) !=
                 QXmlStreamReader::StartElement )
              continue;
            if ( xml.name( ) == "transform" )
            {
              QStringList mL =
                xml.readElementText( ).remove(' ').
                remove('\n').remove('\t').split(",");
              if ( mL.size( ) == 16 )
              {
                for ( unsigned int i = 0; i < 4; i ++ )
                  for ( unsigned int j = 0; j < 4; j ++ )
                    transform( i, j ) =
                      mL[ i * 4 + j].toFloat( );
              }
            }
          }
          vdmCollec->addVDMap( vdmap, transform );
        }
      }
      if ( xml.name( ) == "vdmapCollection" )
      {
        // collectionLoaded = true;
        attributes = xml.attributes( );
        if( attributes.hasAttribute( "vdmapTexture" ) &&
            attributes.hasAttribute("normalTexture") &&
            attributes.hasAttribute( "vdmapsNum" ))
        {
          std::string vdmapTexture = rootPath;
          vdmapTexture.append(
            attributes.value( "vdmapTexture" ).toString( ).toStdString( ));
          std::string normalTexture = rootPath;
          normalTexture.append(
            attributes.value( "normalTexture" ).toString( ).toStdString( ));
          auto vdmap = readVDMap( vdmapTexture, normalTexture );
          if ( !vdmap )
            continue;
          vdmCollec->macroMap( vdmap );
          vdmCollec->vdmapsNum( attributes.value( "vdmapsNum" ).toUInt( ));
          std::vector< Eigen::Matrix4f > transforms;
          while( !xml.atEnd( ) &&  !xml.hasError( ) &&
                 !( xml.name( ) == "vdmapCollection" &&
                    xml.tokenType( ) == QXmlStreamReader::EndElement ))
          {
            Eigen::Matrix4f transform = Eigen::Matrix4f::Identity( );
            xml.readNext( );
            if ( xml.atEnd( ) || xml.tokenType( ) !=
                 QXmlStreamReader::StartElement )
              continue;
            if ( xml.name( ) == "transform" )
            {
              QStringList mL =
                xml.readElementText( ).remove(' ').
                remove('\n').remove('\t').split(",");
              if ( mL.size( ) == 16 )
              {
                for ( unsigned int i = 0; i < 4; i ++ )
                  for ( unsigned int j = 0; j < 4; j ++ )
                    transform( i, j ) =
                      mL[ i * 4 + j].toFloat( );
              }
            }
            transforms.push_back( transform );
          }
          vdmCollec->macroModels( ) = transforms;
        }
      }
    }

    // if ( !collectionLoaded )
    //   vdmCollec->computeMacroMap( );

    return vdmCollec;

  }
#else
  const std::string& /*fileName_*/ )
  {
    std::cerr << "qt5core not supported" << std::endl;
    return nullptr;
  }
#endif

  reto::Texture2D* VDMapReader::_readTexture(
#ifdef NEUROLOTS_USE_TIFF
    const std::string& fileName_, unsigned int& size_ )
  {
    TIFF* tifFile = TIFFOpen( fileName_.c_str( ), "r" );

    TIFFGetField( tifFile, TIFFTAG_IMAGEWIDTH, &size_ );

    unsigned int rowBytesSize = size_ * 3 * 4;
    std::vector< float > pixels( size_ * size_ * 3 );
    unsigned char * buf = ( unsigned char* )_TIFFmalloc( rowBytesSize );

    for ( unsigned int row = 0; row < size_; row++ )
    {
      if ( TIFFReadScanline( tifFile, buf, row ) < 0 )
        break;
      memcpy( &pixels[ row*size_*3 ], buf, rowBytesSize );
    }
    _TIFFfree( buf );
    TIFFClose( tifFile );

    auto texConfig = reto::TextureConfig( );
    texConfig.internalFormat = GL_RGB32F;
    texConfig.format = GL_RGB;
    texConfig.type = GL_FLOAT;
    texConfig.wrapS = GL_CLAMP_TO_EDGE;
    texConfig.wrapT = GL_CLAMP_TO_EDGE;

    return new reto::Texture2D( texConfig, pixels.data( ), size_, size_ );
  }
#else
  const std::string& /*fileName*/, unsigned int& /*size_*/ )
  {
    std::cerr << "libtiff not supported" << std::endl;
    return nullptr;
  }
#endif


  std::vector <float> PCAReader::_readTexture(
#ifdef NEUROLOTS_USE_TIFF
    const std::string& fileName_, unsigned int& size_ )
  {
    TIFF* tifFile = TIFFOpen( fileName_.c_str( ), "r" );

    TIFFGetField( tifFile, TIFFTAG_IMAGEWIDTH, &size_ );

    unsigned int rowBytesSize = size_ * 3 * 4;
    std::vector< float > pixels( size_ * size_ * 3 );
    unsigned char * buf = ( unsigned char* )_TIFFmalloc( rowBytesSize );

    for ( unsigned int row = 0; row < size_; row++ )
    {
      if ( TIFFReadScanline( tifFile, buf, row ) < 0 )
        break;
      memcpy( &pixels[ row*size_*3 ], buf, rowBytesSize );
    }
    _TIFFfree( buf );
    TIFFClose( tifFile );

    auto texConfig = reto::TextureConfig( );
    texConfig.internalFormat = GL_RGB32F;
    texConfig.format = GL_RGB;
    texConfig.type = GL_FLOAT;
    texConfig.wrapS = GL_CLAMP_TO_EDGE;
    texConfig.wrapT = GL_CLAMP_TO_EDGE;

   /* int count=0;
    for( size_t i = 0 ; i < size_ *size_ ; i++ )
    {
      count++;
       std::cout << pixels.data()[i] << " " ;
    }

    std::cout << "size is " << std::to_string(count) << std::endl ;*/

    return pixels;
  }
#else
  const std::string& /*fileName*/, unsigned int& /*size_*/ )
  {
    std::cerr << "libtiff not supported" << std::endl;
    return nullptr;
  }
#endif


}
