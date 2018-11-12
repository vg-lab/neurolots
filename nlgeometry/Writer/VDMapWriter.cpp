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
#include "VDMapWriter.h"

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

#include <iostream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <fstream>

namespace nlgeometry
{

  bool VDMapWriter::writeVDMap( const VDMapPtr vdmap_,
                                const std::string& fileName_ )
  {
    auto vdmTexture = vdmap_->vdmTexture( );
    auto normalTexture = vdmap_->normalTexture( );

    if ( vdmTexture && normalTexture )
    {
      std::string vdmFileName( fileName_ + "_vdm.tiff" );
      std::string normalFileName( fileName_ + "_normal.tiff");

      return _writeTexture( vdmTexture, vdmap_->size( ), vdmFileName ) &&
        _writeTexture( normalTexture, vdmap_->size( ), normalFileName );
    }
    return false;
  }

  bool VDMapWriter::writeVDMapCollection(
    VDMapCollectionPtr vdmapCollection_, const std::string& fileName_,
    bool writeAsCollection_ )
  {
    std::ofstream file;
    file.open( fileName_ );
    if ( !file.is_open( ))
    {
      std::cerr<< "Error: can not create file " << fileName_ << std::endl;
      return false;
    }

    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
         << std::endl << std::endl;
    file << "<vdmaps version=\"0.1\" vdmapsSize=\""
         << vdmapCollection_->vdmapsSize( ) << "\">" << std::endl;

    if ( writeAsCollection_  )
    {
      boost::filesystem::path p( fileName_ );
      std::string relDir( p.stem( ).string( ));
      relDir.append( "_macroMap");
      

      auto vdmap = vdmapCollection_->macroMap( );
      if ( !vdmap ||
           !writeVDMap( vdmap, relDir ))
      {
        return false;
      }
      std::string vdmFile( relDir + "_vdm.tiff" );
      std::string normalFile( relDir + "_normal.tiff");
      file << "  <vdmapCollection vdmapTexture=\""<< vdmFile
           << "\" normalTexture=\"" << normalFile << "\" vdmapsNum=\""
           << vdmapCollection_->vdmapsNum( ) << "\" >\n";
      for ( auto model: vdmapCollection_->macroModels( ))
      {
        file << "    <transform>" << "\n"
             << "      " << model( 0, 0 ) << ", "  << model( 0, 1 ) << ", "
             << model( 0, 2 ) << ", " << model( 0, 3 ) << ",\n"
             << "      " << model( 1, 0 ) << ", "  << model( 1, 1 ) << ", "
             << model( 1, 2 ) << ", " << model( 1, 3 ) << ",\n"
             << "      " << model( 2, 0 ) << ", "  << model( 2, 1 ) << ", "
             << model( 2, 2 ) << ", " << model( 2, 3 ) << ",\n"
             << "      " << model( 3, 0 ) << ", "  << model( 3, 1 ) << ", "
             << model( 3, 2 ) << ", " << model( 3, 3 ) << "\n"
             << "    </transform>\n";
      }
      file << "  </vdmapCollection>" << std::endl;
    }
    else
    {
      boost::filesystem::path p( fileName_ );
      std::string relDir( p.stem( ).string( ) + "_vdmaps/" );
      std::string currentDir;
      if ( !p.parent_path( ).empty( ))
        currentDir.append( p.parent_path( ).string( ) + "/" );
      currentDir.append( relDir );
      boost::filesystem::create_directory( currentDir );
      currentDir.append( relDir );
      auto vdmaps = vdmapCollection_->vdmaps( );
      auto models = vdmapCollection_->models( );
      if ( vdmaps.size( ) == 0 )
      {
        std::cerr << "Error, trying to save an empty vdmaps collection."
                  << std::endl;
        return false;
      }
      for ( unsigned int i = 0; i < vdmaps.size( ); i++ )
      {
        if ( !writeVDMap( vdmaps[i],
                          std::string( currentDir + std::to_string( i ))))
          return false;

        auto& model = models[i];
        std::string vdmFile( relDir + std::to_string( i ) + "_vdm.tiff" );
        std::string normalFile( relDir + std::to_string( i ) + "_normal.tiff");
        file << "  <vdmap vdmapTexture=\""<< vdmFile
             << "\" normalTexture=\"" << normalFile << "\">"<< "\n"
             << "    <transform>" << "\n"
             << "      " << model( 0, 0 ) << ", "  << model( 0, 1 ) << ", "
             << model( 0, 2 ) << ", " << model( 0, 3 ) << ",\n"
             << "      " << model( 1, 0 ) << ", "  << model( 1, 1 ) << ", "
             << model( 1, 2 ) << ", " << model( 1, 3 ) << ",\n"
             << "      " << model( 2, 0 ) << ", "  << model( 2, 1 ) << ", "
             << model( 2, 2 ) << ", " << model( 2, 3 ) << ",\n"
             << "      " << model( 3, 0 ) << ", "  << model( 3, 1 ) << ", "
             << model( 3, 2 ) << ", " << model( 3, 3 ) << "\n"
             << "    </transform>\n"
             << "  </vdmap>" << std::endl;
      }
    }
    file << "</vdmaps>" << std::endl;
    file.close( );
    return true;
  }


  bool VDMapWriter::_writeTexture(
#ifdef NEUROLOTS_USE_TIFF
    reto::Texture2D* texture_,
    unsigned int textureSize_,
    const std::string& fileName_ )
  {
    try{
      unsigned int pixelsSize = textureSize_ * textureSize_ * 3;
      std::vector< float > pixels( pixelsSize );
      texture_->bind( );
      glGetTexImage( texture_->target( ), 0, GL_RGB, GL_FLOAT, pixels.data( ));

      TIFF* tifFile = TIFFOpen( fileName_.c_str( ), "w" );
      if ( tifFile )
      {
        TIFFSetField(tifFile, TIFFTAG_IMAGEWIDTH, textureSize_ );
        TIFFSetField(tifFile, TIFFTAG_IMAGELENGTH, textureSize_ );
        TIFFSetField(tifFile, TIFFTAG_SAMPLESPERPIXEL, 3 );
        TIFFSetField(tifFile, TIFFTAG_BITSPERSAMPLE, 32 );
        TIFFSetField(tifFile, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
        TIFFSetField(tifFile, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        TIFFSetField(tifFile, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        TIFFSetField(tifFile, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

        unsigned int rowBytesSize = textureSize_ * 3 * 4;

        unsigned char * buf = ( unsigned char* )_TIFFmalloc( rowBytesSize );
        for ( unsigned int row = 0; row < textureSize_; row++ )
        {
          memcpy( buf, &pixels[ row*textureSize_*3 ], rowBytesSize );
          if ( TIFFWriteScanline( tifFile, buf, row, 0 ) < 0 )
            break;
        }
        _TIFFfree( buf );
        TIFFClose( tifFile );
      }
      pixels.clear( );
      return true;
    }
    catch( ... )
    {
      std::cerr << "Error writing " << fileName_ << std::endl;
      return false;
    }
  }
#else
    reto::Texture2D* /*texture_*/,
    unsigned int /*textureSize_*/,
    const std::string& /*fileName_*/ )
    {
      std::cerr << "libtiff not supported" << std::endl;
      return false;
    }
#endif

}
