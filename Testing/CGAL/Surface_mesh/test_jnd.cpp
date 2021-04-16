// Copyright (c) 2012-2019 University of Lyon and CNRS (France).
// All rights reserved.
//
// This file is part of MEPP2; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published 
// by the Free Software Foundation; either version 3 of the License, 
// or (at your option) any later version.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#include <CGAL/Cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h> // CGAL Graph traits wrapper
#include "FEVV/Wrappings/Graph_traits_extension_cgal_surface_mesh.h" // FEVV extension for Graph traits wrapper

#include "FEVV/Wrappings/Geometry_traits_cgal_surface_mesh.h" // FEVV geometry wrapper

#include "FEVV/Wrappings/properties_surface_mesh.h"
#include "FEVV/Filters/Generic/generic_reader.hpp"
#include "FEVV/Filters/Generic/generic_writer.hpp"

#include "FEVV/Tools/IO/FileUtilities.hpp"
#include "Testing/Utils/utils_are_meshes_identical.hpp"
#include "Testing/Utils/utils_identical_text_based_files.hpp"

#include "FEVV/Filters/Generic/calculate_face_normals.hpp"
#include "FEVV/Filters/Generic/Manifold/calculate_vertex_normals.hpp"
#include "FEVV/Filters/Generic/Manifold/JustNoticeableDistortion/just_noticeable_distortion.hpp"
#include "FEVV/Filters/Generic/minmax_map.h"
#include "FEVV/Filters/Generic/color_mesh.h"

#include <iostream>
#include <string>


using CGALKernel = CGAL::Cartesian< double >;
using CGALPoint = CGALKernel::Point_3;
using MeshT = CGAL::Surface_mesh< CGALPoint >;


int
main(int argc, const char **argv)
{

  int screen_width = 1920;
  int screen_height = 1080;
  double screen_size = 55.;
  double user_dist = 50.;
  int scene_width = 1080;
  double scene_fov = M_PI * 0.3333;
  int number_of_lights = 16;

  if(argc != 4)
  {
    std::cout
        << "Load a mesh from an input file, compute it's JND, write it to an "
           "output file, then compare the output file with a reference file."
        << std::endl;
    std::cout << "Usage: " << argv[0]
              << "  input_mesh_file output_mesh_file  reference_mesh_file"
              << std::endl;
    std::cout << "Example: " << argv[0]
              << "  armadillo_simplified.obj armadillo.out.obj "
                 "armadillo_jnd_reference.obj"
              << std::endl;
    return EXIT_FAILURE;
  }

  std::string input_file_path = argv[1];
  std::string output_file_path = argv[2];
  std::string reference_file_path = argv[3];

  //--------------------------------------------------

  FEVV::Filters::ColorMeshLUT lut_courbure_clust =
      FEVV::Filters::make_LUT(false);

  ScreenParam screen(screen_width, screen_height, screen_size);
  UserParam user(user_dist);
  SceneParam scene(scene_width, scene_fov);
  // read mesh from file
  FEVV::MeshSurface m;
  FEVV::PMapsContainer pmaps_bag;
  FEVV::Filters::read_mesh(input_file_path, m, pmaps_bag);

  // retrieve point property map (aka geometry)
  auto pm = get(boost::vertex_point, m);
  // Note: the property maps must be extracted from the
  //       property maps bag, and explicitely passed as
  //       parameters to the filter, in order to make
  //       clear what property is used by the filter

  // retrieve or create vertex-color property map
  using VertexColorMap =
      typename FEVV::PMap_traits< FEVV::vertex_color_t,
                                  FEVV::MeshSurface >::pmap_type;
  VertexColorMap v_cm;
  if(has_map(pmaps_bag, FEVV::vertex_color))
  {
    std::cout << "use existing vertex-color map" << std::endl;
    v_cm = get_property_map(FEVV::vertex_color, m, pmaps_bag);
  }
  else
  {
    std::cout << "create vertex-color map" << std::endl;
    v_cm = make_property_map(FEVV::vertex_color, m);
    // store property map in property maps bag
    put_property_map(FEVV::vertex_color, m, pmaps_bag, v_cm);
  }


  // retrieve or create vertex-normal property map
  using FaceNormalMap =
      typename FEVV::PMap_traits< FEVV::face_normal_t,
                                  FEVV::MeshSurface >::pmap_type;
  FaceNormalMap f_nm;
  if(has_map(pmaps_bag, FEVV::face_normal))
  {
    std::cout << "use existing face-normal map" << std::endl;
    f_nm = get_property_map(FEVV::face_normal, m, pmaps_bag);
  }
  else
  {
    std::cout << "create face-normal map" << std::endl;
    f_nm = make_property_map(FEVV::face_normal, m);
    // store property map in property maps bag
    put_property_map(FEVV::face_normal, m, pmaps_bag, f_nm);
    FEVV::Filters::calculate_face_normals(m, pm, f_nm);
  }


  // retrieve or create vertex-normal property map
  using VertexNormalMap =
      typename FEVV::PMap_traits< FEVV::vertex_normal_t,
                                  FEVV::MeshSurface >::pmap_type;
  VertexNormalMap v_nm;
  if(has_map(pmaps_bag, FEVV::vertex_normal))
  {
    std::cout << "use existing vertex-normal map" << std::endl;
    v_nm = get_property_map(FEVV::vertex_normal, m, pmaps_bag);
  }
  else
  {
    std::cout << "create vertex-normal map" << std::endl;
    v_nm = make_property_map(FEVV::vertex_normal, m);

    put_property_map(FEVV::vertex_normal, m, pmaps_bag, v_nm);
    FEVV::Filters::calculate_vertex_normals(m, pm, f_nm, v_nm);
  }

  using JndTypeMap =
      typename FEVV::Vertex_pmap_traits< FEVV::MeshSurface, double >::pmap_type;
  JndTypeMap jnd_m;

  jnd_m = FEVV::make_vertex_property_map< FEVV::MeshSurface, double >(m);


  // apply filter
  //clock_t t_start = clock();
  FEVV::Filters::just_noticeable_distortion_filter(m,
                                                   pm,
                                                   v_nm,
                                                   f_nm,
                                                   jnd_m,
                                                   screen,
                                                   user,
                                                   scene,
                                                   number_of_lights,
                                                   false,
                                                   false);


  double max_jnd, min_jnd;

  FEVV::Filters::compute_min_max_vertices(m, jnd_m, min_jnd, max_jnd);

  FEVV::Filters::color_vertices_from_map(
      m, jnd_m, v_cm, min_jnd, max_jnd, lut_courbure_clust);

  FEVV::Filters::write_mesh(output_file_path, m, pmaps_bag);

  // check output file
  std::cout << "Comparing output file '" << output_file_path
            << "' with reference file '" << reference_file_path << "'..."
            << std::endl;

  if(FEVV::FileUtils::has_extension(output_file_path, ".off") ||
     FEVV::FileUtils::has_extension(output_file_path, ".coff"))
  {
    // use OFF file comparator
    if(!are_meshes_equal(output_file_path, reference_file_path, false))
    {
      std::cout << "Files are different!" << std::endl;
      return EXIT_FAILURE;
    }
  }
  else
  {
    // use text file comparator
    if(!identical_text_based_files(output_file_path, reference_file_path))
    {
      std::cout << "Files are different!" << std::endl;
      return EXIT_FAILURE;
    }
  }

  std::cout << "Files are identical." << std::endl;

  return 0;
}
