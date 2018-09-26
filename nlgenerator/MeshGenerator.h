/**
 * Copyright (c) 2015-2017 GMRV/URJC.
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

#ifndef __NLGENERATOR_MESH_GENERATOR__
#define __NLGENERATOR_MESH_GENERATOR__

#include <nsol/nsol.h>

#include "../nlgeometry/Mesh.h"
#include "JointNode.h"
// #include "VectorizedNode.h"
// #include "Icosphere.h"

#include <nlgenerator/api.h>

namespace nlgenerator
{

  typedef std::unordered_map<
    unsigned int, std::vector< nlgeometry::OrbitalVertexPtr >> NodeIdToVertices;

  typedef std::unordered_map<
    unsigned int, std::vector< unsigned int >> NodeIdToVerticesIds;

  /* \class MeshGenerator */
  class MeshGenerator
  {

  public:

    /**
     * Static method that return a mesh generated from the given morphology
     * @param moprholgy_ to be reconstructed
     * @return a mesh generated from the given morphology
     */
    NLGENERATOR_API
    static nlgeometry::MeshPtr
    generateMesh( nsol::MorphologyPtr morphology_ );

    /**
     * Static method that return a mesh generated from the given morphology
     * changing some morphology params
     * @param moprholgy_ to be reconstructed
     * @param alphaRadius_ param to change the morphology soma radius
     * @param alphaNeurites_ param to change the distance of the morphology
     * neurites with the morphology soma
     * @return a mesh generated from the given morphology
     */
    NLGENERATOR_API
    static nlgeometry::MeshPtr
    generateMesh( nsol::NeuronMorphologyPtr morphology_,
                  float alphaRadius_,
                  const std::vector< float >& alphaNeurites_ );

    /**
     * Static method that return a structure mesh generated from the given
     * morphology
     * @param moprholgy_ to be reconstructed
     * @param nodeToVertices_ structure that keeps the relationship between
     * morphology nodes index and mesh vertices
     * @return a structure mesh generated from the given morphology
     */
    NLGENERATOR_API
    static nlgeometry::MeshPtr
    generateStructureMesh(
      nsol::MorphologyPtr morphology_, NodeIdToVertices& nodeIdToVertices_,
      Eigen::Vector3f color_ = Eigen::Vector3f( 0.0f, 0.0f, 0.0f),
      bool generateNodes_ = false );

    /**
     * Static method that fill the node index to vertices indices structure
     * @param nodeToVertices_ input structure that keeps the relationship
     * between morphology node index and mesh vertices
     * @param nodeIdToVerticesIds_ output structure that keeps the relationship
     * between morphology node index and mesh vertices indices
     */
    NLGENERATOR_API
    static void
    verticesToIndices( NodeIdToVertices& nodeIdToVertices_,
                       NodeIdToVerticesIds& nodeIdToVerticesIds_ );

    /**
     * Static method that fills the buffer with the given value for the vertices
     * related to the given node indices
     * @param nodeIds_ vector of morphology node indices
     * @param nodeIdToVerticesIds_ output structure that keeps the relationship
     * between morphology node index and mesh vertices indices
     * @param buffer_ float buffer to fill
     * @param value_ value to insert in the buffer
     */
    NLGENERATOR_API
    static void
    conformBuffer( std::vector< unsigned int >& nodeIds_,
                   NodeIdToVerticesIds& nodeIdToVerticesIds_,
                   std::vector< float >& buffer_,
                   Eigen::Vector3f value_ );

  protected:
    static nlgeometry::MeshPtr
    _generateMorphology( nsol::MorphologyPtr morphology_ );

    static nlgeometry::MeshPtr
    _generateMophology( nsol::NeuronMorphologyPtr morphology_ );

    static std::unordered_map< nsol::NodePtr, JointNodePtr >
    _vectorizeJoints( const nsol::Sections& sections_ );

    static void _vectorizeJoints(
      std::set< nsol::SectionPtr>& uniqueSections_,
      const nsol::SectionPtr& section_,
      std::unordered_map< nsol::NodePtr, JointNodePtr >& joints_ );

    static nlgeometry::Facets _meshSections(
      const nsol::Sections& sections_,
      std::unordered_map< nsol::NodePtr, JointNodePtr >& joints_ );

    static void _meshSections(
      std::set< nsol::SectionPtr>& uniqueSections_,
      const nsol::SectionPtr& section_,
      std::unordered_map< nsol::NodePtr, JointNodePtr >& joints_,
      nlgeometry::Facets& facets_ );

    static void _vectorizeSections(
      nlgeometry::MeshPtr mesh_,
      const nsol::SectionPtr section_,
      std::set< nsol::SectionPtr>& uniqueSections_,
      NodeIdToVertices& nodeIdToVertices_,
      Eigen::Vector3f color_,
      bool generateNodes_ );

    static nlgeometry::Facets _generateCube(
      nsol::NodePtr node_, NodeIdToVertices& nodeIdToVertices_,
      Eigen::Vector3f color_ );

  }; // class MeshGenerator

} // namespace nlgenerator

#endif
