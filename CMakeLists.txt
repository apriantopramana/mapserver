cmake_minimum_required (VERSION 2.6)

project (MapServer)

include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)



set (MapServer_VERSION_MAJOR 6)
set (MapServer_VERSION_MINOR 3)
set (MapServer_VERSION_REVISION 0)

set(TARGET_VERSION_MAJOR ${MapServer_VERSION_MAJOR})
set(TARGET_VERSION_MINOR ${MapServer_VERSION_MINOR})

MATH(EXPR MapServer_IS_DEV_VERSION "1-${MapServer_VERSION_MINOR}%2")
if(MapServer_IS_DEV_VERSION)
  set (MapServer_VERSION_STRING "${MapServer_VERSION_MAJOR}.${MapServer_VERSION_MINOR}.${MapServer_VERSION_REVISION}")
else(MapServer_IS_DEV_VERSION)
  set (MapServer_VERSION_STRING "${MapServer_VERSION_MAJOR}.${MapServer_VERSION_MINOR}-dev")
endif(MapServer_IS_DEV_VERSION)
MATH(EXPR MapServer_VERSION_NUM "${MapServer_VERSION_MAJOR}*10000+${MapServer_VERSION_MINOR}*100+${MapServer_VERSION_REVISION}")

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
if (APPLE)
  set(CMAKE_FIND_FRAMEWORK "LAST")
endif (APPLE)

macro (ms_link_libraries)
  target_link_libraries(mapserver ${ARGV})
  if(BUILD_STATIC)
    target_link_libraries(mapserver_static ${ARGV})
  endif(BUILD_STATIC)
endmacro()

macro( report_optional_not_found component )
  message(SEND_ERROR "${component} library/component/dependency could not be found.
  HINTS:
  - disable ${component} support by adding -DWITH_${component}=0
  - add the ${component} install directory to the CMAKE_PREFIX_PATH variable (-DCMAKE_PREFIX_PATH=\"/path/to/${component}-install-dir;/path/to/other/dirs\"")
endmacro()
macro( report_mandatory_not_found component )
  message(SEND_ERROR "${component} library/component could not be found and is a mandatory dependency
  HINT:
  - add the ${component} install directory to the CMAKE_PREFIX_PATH variable (-DCMAKE_PREFIX_PATH=\"/path/to/${component}-install-dir;/path/to/other/dirs\"")
endmacro()
macro( report_dependency_error component dependency)
  message(SEND_ERROR "${component} support requires ${dependency} support, however ${dependency} support has been disabled.
  HINTS:
  - re-run with -DWITH_${dependency}=1 (or without -DWITH_${dependency}=0)
  - disable ${component} support by adding -DWITH_${component}=0"
  )
endmacro()

check_function_exists("strrstr"          HAVE_STRRSTR)
check_function_exists("strcasecmp"  HAVE_STRCASECMP)
check_function_exists("strcasestr"  HAVE_STRCASESTR)
check_function_exists("strdup"  HAVE_STRDUP)
check_function_exists("strlcat"  HAVE_STRLCAT)
check_function_exists("strlcpy"  HAVE_STRLCPY)
check_function_exists("strlen"  HAVE_STRLEN)
check_function_exists("strncasecmp"  HAVE_STRNCASECMP)
check_function_exists("vsnprintf"  HAVE_VSNPRINTF)
check_function_exists("lrintf" HAVE_LRINTF)
check_function_exists("lrint" HAVE_LRINT)

check_include_file(dlfcn.h HAVE_DLFCN_H)


check_c_source_compiles("
int main(int argc, char **argv) {
   long x=0,y=0;
   for(x=0;x<5;x++) {
     if(y>1) break;
     y=__sync_fetch_and_add(&x,1);
   }
}" HAVE_SYNC_FETCH_AND_ADD)

if(WITH_FLEX_BISON)
   find_package(BISON)
   find_package(FLEX)

   add_custom_command(
      OUTPUT ${PROJECT_SOURCE_DIR}/maplexer.c
      COMMAND ${FLEX_EXECUTABLE} --nounistd -Pmsyy -i -o ${PROJECT_SOURCE_DIR}/maplexer.c ${PROJECT_SOURCE_DIR}/maplexer.l
      DEPENDS ${PROJECT_SOURCE_DIR}/maplexer.l
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
   )

   add_custom_command(
      OUTPUT ${PROJECT_SOURCE_DIR}/mapparser.c ${PROJECT_SOURCE_DIR}/mapparser.h
      DEPENDS ${PROJECT_SOURCE_DIR}/mapparser.y
      COMMAND ${BISON_EXECUTABLE} -d -o${PROJECT_SOURCE_DIR}/mapparser.c ${PROJECT_SOURCE_DIR}/mapparser.y 
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
   )
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
endif(WITH_FLEX_BISON)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_LINK_INTERFACE_LIBRARY "")

#options suported by the cmake builder
option(WITH_PROJ "Choose if reprojection support should be built in" ON)
option(WITH_KML "Enable native KML output support (requires libxml2 support)" OFF)
option(WITH_SOS "Enable SOS Server support (requires PROJ and libxml2 support)" OFF)
option(WITH_WMS "Enable WMS Server support (requires proj support)" ON)
option(WITH_GD "Choose if (old) GD support should be built in" OFF)
option(WITH_FRIBIDI "Choose if FriBidi glyph shaping support should be built in (usefull for left-to-right languages)" ON)
option(WITH_ICONV "Choose if Iconv Internationalization support should be built in" ON)
option(WITH_CAIRO "Choose if CAIRO  rendering support should be built in (required for SVG and PDF output)" ON)
option(WITH_MYSQL "Choose if MYSQL joining support should be built in" OFF)
option(WITH_FCGI "Choose if FastCGI support should be built in" ON)
option(WITH_GEOS "Choose if GEOS geometry operations support should be built in" ON)
option(WITH_POSTGIS "Choose if Postgis input support should be built in" ON)
option(WITH_GDAL "Choose if GDAL input raster support should be built in" ON)
option(WITH_OGR "Choose if OGR/GDAL input vector support should be built in" ON)
option(WITH_CLIENT_WMS "Enable Client WMS Layer support (requires CURL and GDAL support)" OFF)
option(WITH_CLIENT_WFS "Enable Client WMS Layer support (requires CURL and OGR support)" OFF)
option(WITH_WFS "Enable WFS Server support (requires PROJ and OGR support)" ON)
option(WITH_WCS "Enable WCS Server support (requires PROJ and GDAL support)" ON)
option(WITH_LIBXML2 "Choose if libxml2 support should be built in (used for sos, wcs 1.1,2.0 and wfs 1.1)" ON)
option(WITH_THREADS "Choose if a thread-safe version of libmapserver should be built (only recommended for some mapscripts)" OFF)
option(WITH_GIF "Enable GIF support (for PIXMAP loading)" ON)
option(WITH_PYTHON "Enable Python mapscript support" OFF)
option(WITH_PHP "Enable Python mapscript support" OFF)
option(WITH_PERL "Enable Perl mapscript support" OFF)
option(WITH_RUBY "Enable Ruby mapscript support" OFF)
option(WITH_JAVA "Enable Java mapscript support" OFF)
option(WITH_CSHARP "Enable C# mapscript support" OFF)
option(WITH_POINT_Z_M "include Z and M coordinates in point structure (advanced, not recommended)" OFF)
option(WITH_ORACLESPATIAL "include oracle spatial database input support" OFF)
option(WITH_ORACLE_PLUGIN "include oracle spatial database input support as plugin" OFF)
option(WITH_MSSQL2008 "include mssql 2008 database input support as plugin" OFF)
option(WITH_SDE91 "include ArcSDE version 9.1 support (must specify SDE_INCLUDE_DIR and SDE_LIBRARY_DIR)" OFF)
option(WITH_SDE92 "include ArcSDE version 9.2 support (must specify SDE_INCLUDE_DIR and SDE_LIBRARY_DIR)" OFF)
option(WITH_EXEMPI "include xmp output metadata support" OFF)
option(WITH_XMLMAPFILE "include native xml mapfile support (requires libxslt/libexslt)" OFF)

option(BUILD_STATIC "Also build a static version of mapserver" OFF)
option(LINK_STATIC_LIBMAPSERVER "Link to static version of libmapserver (also for mapscripts)" OFF)
option(WITH_FLEX_BISON "Regenerate parser" OFF)
option(WITH_APACHE_MODULE "include (experimental) support for apache module" OFF)
option(WITH_GENERIC_NINT "generic rounding" OFF)

#TODO: USE_OGL? , USE_SDE, USE_CLUSTER_EXTERNAL USE_CLUSTER_PLUGIN, USE_MSSQL2008, USE_MSSQL2008_PLUGIN
# SIGNORE_MISSING_DATA, CGI_CL_DEBUG_ARGS, EXTRA DEBUG FLAGS?,
# PERLV_ld_DETECT?
if(LINK_STATIC_LIBMAPSERVER)
  set(BUILD_STATIC 1)
  set(MAPSERVER_LIBMAPSERVER mapserver_static)
else(LINK_STATIC_LIBMAPSERVER)
  set(MAPSERVER_LIBMAPSERVER mapserver)
endif(LINK_STATIC_LIBMAPSERVER)

set(agg_SOURCES
renderers/agg/src/agg_arc.cpp
renderers/agg/src/agg_vcgen_dash.cpp
renderers/agg/src/agg_vcgen_contour.cpp
renderers/agg/src/agg_curves.cpp
renderers/agg/src/agg_embedded_raster_fonts.cpp
renderers/agg/src/agg_trans_affine.cpp
renderers/agg/src/agg_vcgen_stroke.cpp
renderers/agg/src/agg_font_freetype.cpp
renderers/agg/src/agg_line_aa_basics.cpp
renderers/agg/src/clipper.cpp
)
include_directories(renderers/agg/include)

if(WIN32)
  set(REGEX_SOURCES ${REGEX_DIR}/regex.c)
  include_directories(${REGEX_DIR})
  add_definitions(-DREGEX_MALLOC -DUSE_GENERIC_MS_NINT -DHAVE_STRING_H)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  set(REGEX_MALLOC 1)
  set(USE_GENERIC_MS_NINT 1)
  set(HAVE_STRING_H 0)
else(WIN32)
  set(REGEX_SOURCES "")
endif(WIN32)

set(mapserver_SOURCES
cgiutil.c mapgeos.c maporaclespatial.c mapsearch.c mapwms.c classobject.c
mapgml.c mapoutput.c mapwmslayer.c layerobject.c mapgraticule.c mapows.c
mapservutil.c mapxbase.c maphash.c mapowscommon.c mapshape.c mapxml.c mapbits.c
maphttp.c mapparser.c mapstring.c mapxmp.c mapcairo.c mapimageio.c
mappluginlayer.c mapsymbol.c mapchart.c mapimagemap.c mappool.c maptclutf.c
mapcluster.c mapio.c mappostgis.c maptemplate.c mapcontext.c mapjoin.c
mappostgresql.c mapthread.c mapcopy.c maplabel.c mapprimitive.c maptile.c
mapcpl.c maplayer.c mapproject.c maptime.c mapcrypto.c maplegend.c
mapprojhack.c maptree.c mapdebug.c maplexer.c mapquantization.c mapunion.c
mapdraw.c maplibxml2.c mapquery.c maputil.c strptime.c mapdrawgdal.c
mapraster.c mapuvraster.c mapdummyrenderer.c mapobject.c maprasterquery.c
mapwcs.c maperror.c mapogcfilter.c mapregex.c mapwcs11.c mapfile.c
mapogcfiltercommon.c maprendering.c mapwcs20.c mapgd.c mapogcsld.c
mapresample.c mapwfs.c mapgdal.c mapogcsos.c mapscale.c mapwfs11.c
mapgeomtransform.c mapogroutput.c mapsde.c mapwfslayer.c mapagg.cpp mapkml.cpp
mapgeomutil.cpp mapkmlrenderer.cpp
mapogr.cpp mapcontour.c ${REGEX_SOURCES})

add_library(mapserver SHARED ${mapserver_SOURCES} ${agg_SOURCES})
set_target_properties( mapserver  PROPERTIES
  VERSION ${MapServer_VERSION_STRING}
  SOVERSION 1
) 
if(BUILD_STATIC)
  add_library(mapserver_static STATIC ${mapserver_SOURCES} ${agg_SOURCES})
  set_target_properties( mapserver_static PROPERTIES
    VERSION ${MapServer_VERSION_STRING}
    SOVERSION 1
  ) 
endif(BUILD_STATIC)

#SOVERSION is not necessarily the same as the
#major version. The rule is that any breakage of the ABI must be
#indicated by incrementing the SOVERSION. So, adding e.g. functions is no
#problem, modifying argument lists or removing functions would required
#the SOVERSION to be incremented. Similar rules hold of course for
#non-opaque data-structures.

add_executable(mapserv mapserv.c)
target_link_libraries(mapserv ${MAPSERVER_LIBMAPSERVER})
add_executable(shp2img shp2img.c)
target_link_libraries(shp2img ${MAPSERVER_LIBMAPSERVER})
add_executable(shptree shptree.c)
target_link_libraries(shptree ${MAPSERVER_LIBMAPSERVER})
add_executable(sortshp sortshp.c)
target_link_libraries(sortshp ${MAPSERVER_LIBMAPSERVER})
add_executable(legend legend.c)
target_link_libraries(legend ${MAPSERVER_LIBMAPSERVER})
add_executable(scalebar scalebar.c)
target_link_libraries(scalebar ${MAPSERVER_LIBMAPSERVER})


find_package(PNG)
if(PNG_FOUND)
  include_directories(${PNG_INCLUDE_DIR})
  ms_link_libraries( ${PNG_LIBRARIES})
  set(USE_PNG 1)
else(PNG_FOUND)
  report_mandatory_not_found(PNG)
endif(PNG_FOUND)

find_package(JPEG)
if(JPEG_FOUND)
  include_directories(${JPEG_INCLUDE_DIR})
  ms_link_libraries( ${JPEG_LIBRARY})
  set(USE_JPEG 1)
else(JPEG_FOUND)
  report_mandatory_not_found(JPEG)
endif(JPEG_FOUND)

find_package(Freetype)
if(NOT FREETYPE_FOUND)
  report_mandatory_not_found(FREETYPE)
endif(NOT FREETYPE_FOUND)
include_directories(${FREETYPE_INCLUDE_DIR})
ms_link_libraries( ${FREETYPE_LIBRARY})


if(WITH_PROJ)
  find_package(Proj)
  if(NOT PROJ_FOUND)
    report_optional_not_found(PROJ)
 else(NOT PROJ_FOUND)
    include_directories(${PROJ_INCLUDE_DIR})
    ms_link_libraries( ${PROJ_LIBRARY})
    set (USE_PROJ 1)
 endif(NOT PROJ_FOUND)
endif (WITH_PROJ)

if(WITH_WMS)
  if(USE_PROJ)
    set(USE_WMS_SVR 1)
  else(USE_PROJ)
    report_dependency_error(WMS PROJ)
  endif(USE_PROJ)
endif(WITH_WMS)


if(WITH_GD)
  find_package(GD)
  if(NOT GD_FOUND)
    report_optional_not_found(GD)
 else(NOT GD_FOUND)
    include_directories(${GD_INCLUDE_DIR})
    ms_link_libraries( ${GD_LIBRARY})
    set (USE_GD 1)
    if(GD_SUPPORTS_PNG)
      set(USE_GD_PNG 1)
    endif(GD_SUPPORTS_PNG)
    if(GD_SUPPORTS_GIF)
      set(USE_GD_GIF 1)
    endif(GD_SUPPORTS_GIF)
    if(GD_SUPPORTS_JPEG)
      set(USE_GD_JPEG 1)
    endif(GD_SUPPORTS_JPEG)
    if(GD_SUPPORTS_FREETYPE)
      set(USE_GD_FREETYPE 1)
    endif(GD_SUPPORTS_FREETYPE)
 endif(NOT GD_FOUND)
endif (WITH_GD)

if(WITH_FRIBIDI)
  find_package(FriBiDi)
  if(NOT FRIBIDI_FOUND)
    report_optional_not_found(FRIBIDI)
 else(NOT FRIBIDI_FOUND)
    include_directories(${FRIBIDI_INCLUDE_DIR})
    ms_link_libraries( ${FRIBIDI_LIBRARY})
    set (USE_FRIBIDI 1)
    if(FRIBIDI_LEGACY)
      message(WARNING "Old Fribidi found, upgrade recommended")
    endif(FRIBIDI_LEGACY)
 endif(NOT FRIBIDI_FOUND)
endif (WITH_FRIBIDI)

if(WITH_ICONV)
  find_package(ICONV)
  if(ICONV_FOUND)
    include_directories(${ICONV_INCLUDE_DIR})
    ms_link_libraries( ${ICONV_LIBRARY})
    set (USE_ICONV 1)
  else(ICONV_FOUND)
    report_optional_not_found(ICONV)
  endif(ICONV_FOUND)
endif (WITH_ICONV)

if(WITH_GENERIC_NINT)
   set(USE_GENERIC_MS_NINT 1)
endif(WITH_GENERIC_NINT)


if(WITH_CAIRO)
  find_package(Cairo)
  if(CAIRO_FOUND)
    include_directories(${CAIRO_INCLUDE_DIRS})
    ms_link_libraries( ${CAIRO_LIBRARY})
    set (USE_CAIRO 1)
  else(CAIRO_FOUND)
    report_optional_not_found(CAIRO)
  endif(CAIRO_FOUND)
endif (WITH_CAIRO)

if(WITH_MYSQL)
  find_package(MySQL)
  if(MYSQL_FOUND)
    include_directories(${MYSQL_INCLUDE_DIR})
    ms_link_libraries( ${MYSQL_LIBRARY})
    set (USE_MYSQL 1)
  else(MYSQL_FOUND)
    report_optional_not_found(MYSQL)
  endif(MYSQL_FOUND)
endif (WITH_MYSQL)

if(WITH_ORACLE_PLUGIN AND WITH_ORACLESPATIAL)
  message(SEND_ERROR "WITH_ORACLESPATIAL and WITH_ORACLE_PLUGIN cannot be used simultaneously")
endif(WITH_ORACLE_PLUGIN AND WITH_ORACLESPATIAL)

if(WITH_ORACLESPATIAL OR WITH_ORACLE_PLUGIN)
  if(NOT DEFINED ENV{ORACLE_HOME})
    MESSAGE( SEND_ERROR "ORACLE_HOME environment variable not set, needed for detection")
  endif()
  find_package(Oracle)
  if(ORACLE_FOUND)
    include_directories(${ORACLE_INCLUDE_DIR})
  else(ORACLE_FOUND)
    report_optional_not_found(ORACLESPATIAL)
    #FIXME: error message here could be misleading, only states ORACLESPATIAL whereas
    #the request could have been for ORACLE_PLUGIN
  endif(ORACLE_FOUND)
endif(WITH_ORACLESPATIAL OR WITH_ORACLE_PLUGIN)

if(ORACLE_FOUND AND WITH_ORACLESPATIAL)
    ms_link_libraries( ${ORACLE_LIBRARY})
    set (USE_ORACLESPATIAL 1)
endif(ORACLE_FOUND AND WITH_ORACLESPATIAL)

if(ORACLE_FOUND AND WITH_ORACLE_PLUGIN)
    add_library(msplugin_oracle MODULE maporaclespatial.c)
    target_link_libraries(msplugin_oracle ${ORACLE_LIBRARY} ${MAPSERVER_LIBMAPSERVER})
    set_target_properties(msplugin_oracle PROPERTIES COMPILE_FLAGS -DUSE_ORACLE_PLUGIN)
	set (USE_ORACLE_PLUGIN 1)
endif(ORACLE_FOUND AND WITH_ORACLE_PLUGIN)

if(WITH_MSSQL2008)
  find_package(ODBC)
  if(ODBC_FOUND)
    include_directories(${ODBC_INCLUDE_DIR})
	add_library(msplugin_mssql2008 MODULE mapmssql2008.c)
	target_link_libraries(msplugin_mssql2008 ${ODBC_LIBRARY} ${MAPSERVER_LIBMAPSERVER})
    set_target_properties(msplugin_mssql2008 PROPERTIES COMPILE_FLAGS "-DUSE_MSSQL2008_PLUGIN -DUSE_MSSQL2008")
	set (USE_MSSQL2008 1)
  else(ODBC_FOUND)
    report_optional_not_found(ODBC)
  endif(ODBC_FOUND)
endif(WITH_MSSQL2008)

if(WITH_SDE91)
  if(SDE_INCLUDE_DIR AND SDE_LIBRARY_DIR)
    include_directories(${SDE_INCLUDE_DIR})
	add_library(msplugin_sde91 MODULE mapsde.c mapthread.c)
	target_link_libraries(msplugin_sde91 ${SDE_LIBRARY_DIR}/pe91.lib ${SDE_LIBRARY_DIR}/sg91.lib ${SDE_LIBRARY_DIR}/sde91.lib ${MAPSERVER_LIBMAPSERVER})
    set_target_properties(msplugin_sde91 PROPERTIES COMPILE_FLAGS "-DUSE_SDE")
	set (USE_SDE91 1)
  else(SDE_INCLUDE_DIR AND SDE_LIBRARY_DIR)
     MESSAGE( SEND_ERROR "Must specify SDE_INCLUDE_DIR and SDE_LIBRARY_DIR")
  endif(SDE_INCLUDE_DIR AND SDE_LIBRARY_DIR)
endif(WITH_SDE91)

if(WITH_SDE92)
  if(SDE_INCLUDE_DIR AND SDE_LIBRARY_DIR)
    include_directories(${SDE_INCLUDE_DIR})
	add_library(msplugin_sde92 MODULE mapsde.c mapthread.c)
	target_link_libraries(msplugin_sde92 ${SDE_LIBRARY_DIR}/pe.lib ${SDE_LIBRARY_DIR}/sg.lib ${SDE_LIBRARY_DIR}/sde.lib ${MAPSERVER_LIBMAPSERVER})
    set_target_properties(msplugin_sde92 PROPERTIES COMPILE_FLAGS "-DUSE_SDE")
	set (USE_SDE92 1)
  else(SDE_INCLUDE_DIR AND SDE_LIBRARY_DIR)
     MESSAGE( SEND_ERROR "Must specify SDE_INCLUDE_DIR and SDE_LIBRARY_DIR")
  endif(SDE_INCLUDE_DIR AND SDE_LIBRARY_DIR)
endif(WITH_SDE92)

if(WITH_FCGI)
  find_package(FCGI)
  if(FCGI_FOUND)
    include_directories(${FCGI_INCLUDE_DIR})
    target_link_libraries(mapserv ${FCGI_LIBRARY})
    set (USE_FASTCGI 1)
  else(FCGI_FOUND)
    report_optional_not_found(FCGI)
  endif(FCGI_FOUND)
endif (WITH_FCGI)

if(WITH_GEOS)
  find_package(GEOS)
  if(GEOS_FOUND)
    include_directories(${GEOS_INCLUDE_DIR})
    ms_link_libraries( ${GEOS_LIBRARY})
    set (USE_GEOS 1)
  else(GEOS_FOUND)
    report_optional_not_found(GEOS)
  endif(GEOS_FOUND)
endif (WITH_GEOS)

if(WITH_POSTGIS)
  find_package(PostgreSQL)
  if(POSTGRESQL_FOUND)
    #uppercase our variables
    if ( NOT DEFINED(POSTGRESQL_LIBRARY) )
      set( POSTGRESQL_LIBRARY ${PostgreSQL_LIBRARY})
    endif()
    if ( NOT DEFINED(POSTGRESQL_INCLUDE_DIR) )
      set( POSTGRESQL_INCLUDE_DIR ${PostgreSQL_INCLUDE_DIR})
    endif()
    include_directories(${POSTGRESQL_INCLUDE_DIR})
    ms_link_libraries( ${POSTGRESQL_LIBRARY})
    set (USE_POSTGIS 1)
  else(POSTGRESQL_FOUND)
    report_optional_not_found(POSTGIS)
  endif(POSTGRESQL_FOUND)
endif (WITH_POSTGIS)

if(WITH_GDAL)
  find_package(GDAL)
  if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR})
    ms_link_libraries( ${GDAL_LIBRARY})
    set (USE_GDAL 1)
  else(GDAL_FOUND)
    report_optional_not_found(GDAL)
  endif(GDAL_FOUND)
endif (WITH_GDAL)

if(WITH_OGR)
  if(GDAL_FOUND)
    set (USE_OGR 1)
  else(GDAL_FOUND)
    find_package(GDAL)
    if(GDAL_FOUND)
      include_directories(${GDAL_INCLUDE_DIR})
      ms_link_libraries( ${GDAL_LIBRARY})
      set (USE_OGR 1)
    else(GDAL_FOUND)
      message(SEND_ERROR "GDAL library could not be found and is needed for OGR support.
      HINTS:
      - disable OGR support by adding -DWITH_OGR=0
      - add the GDAL install directory to the CMAKE_PREFIX_PATH variable (-DCMAKE_PREFIX_PATH=\"/path/to/${component}-install-dir;/path/to/other/dirs\"")
    endif(GDAL_FOUND)
  endif(GDAL_FOUND)
endif(WITH_OGR)

if((WITH_CLIENT_WMS) OR (WITH_CLIENT_WFS))
   find_package(CURL)
   if(CURL_FOUND)
     include_directories(${CURL_INCLUDE_DIR})
     ms_link_libraries( ${CURL_LIBRARY})
     set(USE_CURL 1)
   else(CURL_FOUND)
      if(WITH_CLIENT_WFS)
        report_dependency_error(CLIENT_WFS CURL)
      endif(WITH_CLIENT_WFS)
      if(WITH_CLIENT_WMS)
        report_dependency_error(CLIENT_WMS CURL)
      endif(WITH_CLIENT_WMS)
   endif(CURL_FOUND)
endif((WITH_CLIENT_WMS) OR (WITH_CLIENT_WFS))

if(WITH_CLIENT_WMS)
  if(USE_GDAL AND USE_PROJ)
    set(USE_WMS_LYR 1)
  else(USE_GDAL AND USE_PROJ)
    if(NOT USE_GDAL)
      report_dependency_error(CLIENT_WMS GDAL)
    endif(NOT USE_GDAL)
    if( NOT USE_PROJ)
      report_dependency_error(CLIENT_WMS PROJ)
    endif(NOT USE_PROJ)
  endif(USE_GDAL AND USE_PROJ)
endif(WITH_CLIENT_WMS)

if(WITH_CLIENT_WFS)
  if(USE_OGR AND USE_PROJ)
    set(USE_WFS_LYR 1)
  else(USE_OGR AND USE_PROJ)
    if(NOT USE_OGR)
      report_dependency_error(CLIENT_WFS OGR)
    endif(NOT USE_OGR)
    if( NOT USE_PROJ)
      report_dependency_error(CLIENT_WFS PROJ)
    endif(NOT USE_PROJ)
  endif(USE_OGR AND USE_PROJ)
endif(WITH_CLIENT_WFS)

if(WITH_WFS)
  if(USE_PROJ AND USE_OGR)
    set(USE_WFS_SVR 1)
    #TODO: set WFS_USE_LIBXML2 ?
  else(USE_PROJ AND USE_OGR)
    if( NOT USE_OGR)
      report_dependency_error(WFS OGR)
    endif( NOT USE_OGR)
    if( NOT USE_PROJ)
      report_dependency_error(WFS PROJ)
    endif( NOT USE_PROJ)
  endif(USE_PROJ AND USE_OGR)
endif(WITH_WFS)

if(WITH_WCS)
  if(USE_PROJ AND USE_GDAL)
    set(USE_WCS_SVR 1)
  else(USE_PROJ AND USE_GDAL)
    if(NOT USE_GDAL)
      report_dependency_error(WCS GDAL)
    endif(NOT USE_GDAL)
    if(NOT USE_PROJ)
      report_dependency_error(WCS PROJ)
    endif(NOT USE_PROJ)
  endif(USE_PROJ AND USE_GDAL)
endif(WITH_WCS)

if(WITH_LIBXML2)
  find_package(LibXml2)
  if(LIBXML2_FOUND)
    include_directories(${LIBXML2_INCLUDE_DIR})
    if(NOT DEFINED(LIBXML2_LIBRARY))
      set(LIBXML2_LIBRARY ${LIBXML2_LIBRARIES})
    endif()
    ms_link_libraries( ${LIBXML2_LIBRARY})
    set (USE_LIBXML2 1)
  else(LIBXML2_FOUND)
    report_optional_not_found(LIBXML2)
  endif(LIBXML2_FOUND)
endif (WITH_LIBXML2)

if( USE_WCS_SVR AND NOT USE_LIBXML2 )
   message(WARNING "WCS 1.1 and 2.0 require libxml2 support but it was not found. WCS 1.1 and 2.0 will not be supported by this build")
endif( USE_WCS_SVR AND NOT USE_LIBXML2 )
if( USE_WFS_SVR AND NOT USE_LIBXML2 )
   message(WARNING "WFS 1.1 requires libxml2 support but it was not found. WFS 1.1 will not be supported by this build")
endif( USE_WFS_SVR AND NOT USE_LIBXML2 )

if(WITH_SOS)
  if(USE_PROJ AND USE_LIBXML2)
    set(USE_SOS_SVR 1)
  else(USE_PROJ AND USE_LIBXML2)
    if(NOT USE_LIBXML2)
      report_dependency_error(SOS LIBXML2)
    endif(NOT USE_LIBXML2)
    if(NOT USE_PROJ)
      report_dependency_error(SOS PROJ)
    endif(NOT USE_PROJ)
  endif(USE_PROJ AND USE_LIBXML2)
endif(WITH_SOS)

if(WITH_POINT_Z_M)
  set(USE_POINT_Z_M 1)
endif(WITH_POINT_Z_M)

if(WITH_KML)
  if(USE_LIBXML2)
    set(USE_KML 1)
  else(USE_LIBXML2)
    report_dependency_error(KML LIBXML2)
  endif(USE_LIBXML2)
endif(WITH_KML)

if(WITH_THREADS)
  set( CMAKE_THREAD_PREFER_PTHREAD 1 )
  find_package(Threads)
  if (THREADS_FOUND)
    ms_link_libraries( ${CMAKE_THREAD_LIBS_INIT})
    set(USE_THREAD 1)
  endif (THREADS_FOUND)
endif(WITH_THREADS)

if(WITH_XMLMAPFILE)
   if(NOT USE_LIBXML2)
    report_dependency_error(KML LIBXML2)
   endif(NOT USE_LIBXML2)
  
  #check for xslt and exslt
  find_package(LibXslt)
  if(LIBXSLT_FOUND)
    if(NOT DEFINED(LIBXSLT_LIBRARY))
      set(LIBXSLT_LIBRARY ${LIBXSLT_LIBRARIES})
    endif()
    include_directories(${LIBXSLT_INCLUDE_DIR})
    ms_link_libraries( ${LIBXSLT_LIBRARY} ${LIBXSLT_EXSLT_LIBRARY})
    set(USE_XMLMAPFILE 1)
  else(LIBXSLT_FOUND)
    message(SEND_ERROR "Xml Mapfile support requires XSLT support which was not found.
    HINTS:
    - add the libxslt install directory to the CMAKE_PREFIX_PATH variable (-DCMAKE_PREFIX_PATH=\"/path/to/libxslt-install-dir;/path/to/other/dirs\"
    - disable Xml Mapfile support by adding -DWITH_XMLMAPFILE=0"
    )
  endif(LIBXSLT_FOUND)
endif(WITH_XMLMAPFILE)

if(WITH_GIF)
  find_package(GIF)
  if(GIF_FOUND)
    include_directories(${GIF_INCLUDE_DIR})
    ms_link_libraries( ${GIF_LIBRARY})
    set(USE_GIF 1)
  else(GIF_FOUND)
    report_optional_not_found(GIF)
  endif(GIF_FOUND)
endif(WITH_GIF)

if(WITH_EXEMPI)
  find_package(Exempi)
  if(LIBEXEMPI_FOUND)
    include_directories(${LIBEXEMPI_INCLUDE_DIR})
    ms_link_libraries( ${LIBEXEMPI_LIBRARY})
    set(USE_EXEMPI 1)
  else(LIBEXEMPI_FOUND)
    report_optional_not_found(EXEMPI)
  endif(LIBEXEMPI_FOUND)
endif(WITH_EXEMPI)

if(WITH_PYTHON)
   add_subdirectory("mapscript/python")
   set(USE_PYTHON_MAPSCRIPT 1)
endif(WITH_PYTHON)

if(WITH_PHP)
   add_subdirectory("mapscript/php")
   set(USE_PHP_MAPSCRIPT 1)
endif(WITH_PHP)

if(WITH_APACHE_MODULE)
   add_subdirectory("apache")
   set(USE_APACHE_MODULE 1)
endif(WITH_APACHE_MODULE)


if(WITH_PERL)
   add_subdirectory("mapscript/perl")
   set(USE_PERL_MAPSCRIPT 1)
endif(WITH_PERL)

if(WITH_RUBY)
   add_subdirectory("mapscript/ruby")
   set(USE_RUBY_MAPSCRIPT 1)
endif(WITH_RUBY)

if(WITH_JAVA)
   add_subdirectory("mapscript/java")
   set(USE_JAVA_MAPSCRIPT 1)
endif(WITH_JAVA)

if(WITH_CSHARP)
   add_subdirectory("mapscript/csharp")
   set(USE_CSHARP_MAPSCRIPT 1)
endif(WITH_CSHARP)

if(UNIX)
ms_link_libraries( ${CMAKE_DL_LIBS} m )
endif(UNIX)

if (WIN32)
ms_link_libraries( ${MS_EXTERNAL_LIBS})
endif (WIN32)

configure_file (
  "${PROJECT_SOURCE_DIR}/mapserver-config.h.in"
  "${PROJECT_BINARY_DIR}/mapserver-config.h"
  )

configure_file (
  "${PROJECT_SOURCE_DIR}/mapserver-version.h.in"
  "${PROJECT_BINARY_DIR}/mapserver-version.h"
  )



macro(status_optional_component component enabled libpath)
  if("${enabled}" EQUAL "1")
    message(STATUS "  * ${component}: ${libpath}")
  else()
    message(STATUS "  * ${component}: disabled")
  endif()
endmacro()
macro(status_optional_feature feature enabled)
  if("${enabled}" EQUAL "1")
    message(STATUS "  * ${feature}: ENABLED")
  else()
    message(STATUS "  * ${feature}: disabled")
  endif()
endmacro()

message(STATUS "* Summary of configured options for this build")
message(STATUS " * Mandatory components")
message(STATUS "  * png: ${PNG_LIBRARY}")
message(STATUS "  * jpeg: ${JPEG_LIBRARY}")
message(STATUS "  * freetype: ${FREETYPE_LIBRARY}")
message(STATUS " * Optional components")
status_optional_component("GDAL" "${USE_GDAL}" "${GDAL_LIBRARY}")
status_optional_component("OGR" "${USE_OGR}" "${GDAL_LIBRARY}")
status_optional_component("GD" "${USE_GD}" "${GD_LIBRARY}")
status_optional_component("GIF" "${USE_GIF}" "${GIF_LIBRARY}")
status_optional_component("MYSQL" "${USE_MYSQL}" "${MYSQL_LIBRARY}")
status_optional_component("FRIBIDI" "${USE_FRIBIDI}" "${FRIBIDI_LIBRARY}")
status_optional_component("GIF" "${USE_GIF}" "${GIF_LIBRARY}")
status_optional_component("CAIRO" "${USE_CAIRO}" "${CAIRO_LIBRARY}")
status_optional_component("CURL" "${USE_CURL}" "${CURL_LIBRARY}")
status_optional_component("PROJ" "${USE_PROJ}" "${PROJ_LIBRARY}")
status_optional_component("LIBXML2" "${USE_LIBXML2}" "${LIBXML2_LIBRARY}")
status_optional_component("POSTGIS" "${USE_POSTGIS}" "${POSTGRESQL_LIBRARY}")
status_optional_component("GEOS" "${USE_GEOS}" "${GEOS_LIBRARY}")
status_optional_component("FastCGI" "${USE_FASTCGI}" "${FCGI_LIBRARY}")
if(USE_ORACLESPATIAL OR USE_ORACLE_PLUGIN)
  if(USE_ORACLESPATIAL)
    status_optional_component("Oracle Spatial" "${USE_ORACLESPATIAL}" "${ORACLE_LIBRARY}")
  else(USE_ORACLESPATIAL)
    status_optional_component("Oracle Spatial (Built as plugin)" "${USE_ORACLE_PLUGIN}" "${ORACLE_LIBRARY}")
  endif(USE_ORACLESPATIAL)
else(USE_ORACLESPATIAL OR USE_ORACLE_PLUGIN)
  status_optional_component("Oracle Spatial" "" "${ORACLE_LIBRARY}")
endif(USE_ORACLESPATIAL OR USE_ORACLE_PLUGIN)
if(USE_MSSQL2008)
  status_optional_component("MSSQL 2008 (Built as plugin)" "${USE_MSSQL2008}" "${ODBC_LIBRARY}")
endif(USE_MSSQL2008)
if(USE_SDE91)
  status_optional_component("SDE 91 (Built as plugin)" "${USE_SDE91}" "${SDE_LIBRARY_DIR}")
endif(USE_SDE91)
if(USE_SDE92)
  status_optional_component("SDE 92 (Built as plugin)" "${USE_SDE92}" "${SDE_LIBRARY_DIR}")
endif(USE_SDE92)
status_optional_component("Exempi XMP" "${USE_EXEMPI}" "${LIBEXEMPI_LIBRARY}")
message(STATUS " * Optional features")
status_optional_feature("WMS SERVER" "${USE_WMS_SVR}")
status_optional_feature("WFS SERVER" "${USE_WFS_SVR}")
status_optional_feature("WCS SERVER" "${USE_WCS_SVR}")
status_optional_feature("SOS SERVER" "${USE_SOS_SVR}")
status_optional_feature("WMS CLIENT" "${USE_WMS_LYR}")
status_optional_feature("WFS CLIENT" "${USE_WFS_LYR}")
status_optional_feature("ICONV" "${USE_ICONV}")
status_optional_feature("Thread-safety support" "${USE_THREAD}")
status_optional_feature("KML output" "${USE_KML}")
status_optional_feature("Z+M point coordinate support" "${USE_POINT_Z_M}")
status_optional_feature("XML Mapfile support" "${USE_XMLMAPFILE}")

message(STATUS " * Mapscripts")
status_optional_feature("Python" "${USE_PYTHON_MAPSCRIPT}")
status_optional_feature("PHP" "${USE_PHP_MAPSCRIPT}")
status_optional_feature("PERL" "${USE_PERL_MAPSCRIPT}")
status_optional_feature("RUBY" "${USE_RUBY_MAPSCRIPT}")
status_optional_feature("JAVA" "${USE_JAVA_MAPSCRIPT}")
status_optional_feature("C#" "${USE_CSHARP_MAPSCRIPT}")
status_optional_feature("Apache Module (Experimental)" "${USE_APACHE_MODULE}")

include_directories("${PROJECT_BINARY_DIR}")

if(WIN32)
set(REGEX_MALLOC 1)
set(USE_GENERIC_MS_NINT 1)
endif(WIN32)

#INSTALL(FILES mapserver-api.h ${PROJECT_BINARY_DIR}/mapserver-version.h DESTINATION include)
if(USE_ORACLE_PLUGIN)
   INSTALL(TARGETS msplugin_oracle DESTINATION lib)
endif(USE_ORACLE_PLUGIN)

if(USE_MSSQL2008)
   INSTALL(TARGETS msplugin_mssql2008 DESTINATION lib)
endif(USE_MSSQL2008)

if(USE_SDE91)
   INSTALL(TARGETS msplugin_sde91 DESTINATION lib)
endif(USE_SDE91)

if(USE_SDE92)
   INSTALL(TARGETS msplugin_sde92 DESTINATION lib)
endif(USE_SDE92)

INSTALL(TARGETS sortshp shptree shp2img mapserv mapserver RUNTIME DESTINATION bin LIBRARY DESTINATION lib)
if(BUILD_STATIC)
   INSTALL(TARGETS mapserver_static DESTINATION lib)
endif(BUILD_STATIC)

