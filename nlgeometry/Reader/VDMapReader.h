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
#ifndef __NLGEOMETRY_VDMAP_READER__
#define __NLGEOMETRY_VDMAP_READER__

#include "../VDMap.h"

#include <reto/reto.h>

#include <nlgeometry/api.h>

namespace nlgeometry
{
  class VDMapReader
  {
  public:

    NLGEOMETRY_API
    static VDMapPtr readVDMap( const std::string& vdmFile_,
                               const std::string& normalFile_ );

    NLGEOMETRY_API
    static VDMapCollectionPtr readVDMapCollection(
      const std::string& fileName_ );

  protected:

    static reto::Texture2D* _readTexture( const std::string& fileName_,
                                          unsigned int& size_ );

  };

  class PCAReader
  {
    public:
    static VDMapPtr readPCATexture(const std::string& pcaFile);

    static nlgeometry::PCACompMapPtr readMacrotextureComponents(const std::string* pcaComponentsPath, int& numComponentes);

    static std::vector <void *> _readTexture( const std::string& fileName_,
                                          unsigned int& size_ );
  };
}

#endif
