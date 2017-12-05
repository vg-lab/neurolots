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
#include <iostream>
#include <chrono>
#include <thread>

#include <nlrender/version.h>
#include <rockets/server.h>
#include <lexis/render/ImageJPEG.h>

const auto wsProtocol = "rockets";

using namespace rockets::http;
nlrender::Version version;


class Handler
{

public:
  rockets::ws::Response processWebsocketMessage( const std::string& message )
  {
    std::cout << message << std::endl;
    return nullptr;
  }
};

std::string getCameraSchema( void );
std::string getCameraData( void );
std::string getImageSchema( void );
std::string getImageData( void );

Handler handler;

int main( int /*argc*/, char** /*argv*/ )
{
  std::cout << "neurolots example: Rockets Renderer" << std::endl;
  rockets::Server* httpServer = new rockets::Server( ":8080", wsProtocol );
  std::cout << "Server uri " << httpServer->getURI( ) << std::endl;

  // httpServer->handleGET( "v1/version/schema", version );

  httpServer->handleText(std::bind(&Handler::processWebsocketMessage,
                                   handler, std::placeholders::_1));
  httpServer->handle(Method::GET, "v1/version/schema",
                     [&](const Request&) {
                       return make_ready_response(
                         Code::OK, version.getSchema( ),
                         "application/json");
                     });
  httpServer->handle(Method::GET, "v1/version",
                     [&](const Request&) {
                       return make_ready_response(
                         Code::OK, version.toJSON( ), "application/json" );
                     });
  httpServer->handle(Method::GET, "v1/camera/schema",
                     [&](const Request&) {
                       return make_ready_response(
                         Code::OK, getCameraSchema( ), "application/json" );
                     });
  httpServer->handle(Method::GET, "v1/camera",
                     [&](const Request&) {
                       return make_ready_response(
                         Code::OK, getCameraData( ), "application/json" );
                     });
  httpServer->handle(Method::PUT, "v1/camera",
                     [&](const Request& request) {
                       std::cout << request.body << std::endl;
                       return make_ready_response(
                         Code::OK );
                     });
  httpServer->handle(Method::GET, "v1/image-jpeg/schema",
                     [&](const Request&) {
                       return make_ready_response(
                         Code::OK, getImageSchema( ), "application/json" );
                     });
  httpServer->handle(Method::GET, "v1/image-jpeg",
                     [&](const Request&) {
                       return make_ready_response(
                         Code::OK, getImageData( ), "application/json" );
                     });
  // size_t size = 0;
  // char * data = nullptr;
  while( true )
  {
    // httpServer->broadcastBinary( data, size );
    httpServer->process( 0 );
    std::this_thread::sleep_for( std::chrono::milliseconds(1));
  }

  return 0;
}

std::string getImageSchema( void )
{
    return R"({
    "$schema": "http://json-schema.org/schema#",
    "title": "ImageJPEG",
    "description": "Class ImageJPEG",
    "type": "object",
    "additionalProperties": false,
    "properties": {
        "data": {
            "type": "string",
            "media": {
                "binaryEncoding": "base64"
            }
        }
    }
})";
}

std::string getImageData( char * data )
{
  return R"({
      "data": " "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    })";
}

std::string getCameraData( void )
{
  return R"({
      "origin": [0.0, 0.0, 0.0],
        "look_at": [1.0,0.0,0.0],
    })";
}

std::string getCameraSchema( void )
{
  return R"({
    "$schema": "http://json-schema.org/schema#",
    "title": "Camera",
    "description": "Class Camera of namespace ['brayns','v1']",
    "type": "object",
    "additionalProperties": false,
    "properties": {
        "origin": {
            "type": "array",
            "minItems": 3,
            "maxItems": 3,
            "items": {
                "type": "number"
            }
        },
        "look_at": {
            "type": "array",
            "minItems": 3,
            "maxItems": 3,
            "items": {
                "type": "number"
            }
        },
        "up": {
            "type": "array",
            "minItems": 3,
            "maxItems": 3,
            "items": {
                "type": "number"
            }
        },
        "field_of_view": {
            "type": "number"
        },
        "aperture": {
            "type": "number"
        },
        "focal_length": {
            "type": "number"
        },
        "stereo_mode": {
            "$schema": "http://json-schema.org/schema#",
            "title": "CameraStereoMode",
            "description": "Enum CameraStereoMode of type uint",
            "type": "string",
            "additionalProperties": false,
            "enum": [
                "none",
                "left",
                "right",
                "side_by_side"
            ]
        },
        "eye_separation": {
            "type": "number"
        }
    }
})";
}
