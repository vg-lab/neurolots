#ifndef __NEUROLOTS_SHADER__
#define __NEUROLOTS_SHADER__

#include <string>

#include <neurolots/nlrender/api.h>

namespace neurolots
{

  class Shader
  {

  public:
    typedef enum
    {
      TRIANGLES_VERTEX = 0,
      TRIANGLES_TESS_CONTROL,
      TRIANGLES_TESS_EVALUATION,
      TRIANGLES_GEOMETRY,
      TRIANGLES_FRAGMENT,
      QUADS_VERTEX,
      QUADS_TESS_CONTROL,
      QUADS_TESS_EVALUATION,
      QUADS_GEOMETRY,
      QUADS_FRAGMENT
    } TShader;

    NLRENDER_API
    Shader( unsigned int type_, const std::string& source_ );

    NLRENDER_API
    ~Shader( void );

    NLRENDER_API
    unsigned int& id( void );

    NLRENDER_API
    unsigned int& type( void );

    NLRENDER_API
    static std::string sourceCode( TShader type_ );

  private:

    void _init( const std::string& source_ );

    void _compileShader( const std::string& source_ );

    unsigned int _id;
    unsigned int _type;

  };

  typedef Shader* ShaderPtr;

} // end namespace neurolots

#endif // __NEUROLOTS_SHADER__

// EOF
