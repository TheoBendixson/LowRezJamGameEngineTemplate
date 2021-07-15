
// TODO: (Ted)  Make it we can pass the viewport size into the prjection matrix.

const GLchar *VertexSource =
    "attribute vec2 Position;        			                    \n"
    "attribute vec2 InTextureCoordinate;                            \n"
    "varying vec2 TextureCoordinate;                                \n"
    "void main()                                				    \n"
    "{                                          				    \n"
	"	mat4 ProjectionMatrix = mat4(2.0/1024.0, 0.0, 0.0, -1.0,    \n"
	"								 0.0, 2.0/1024.0, 0.0, -1.0,    \n"
	"								 0.0, 0.0, -1.0, 0.0,		    \n"
	"								 0.0, 0.0, 0.0, 1.0);		    \n"
    "   gl_Position = vec4(Position.xy, 0.0, 1.0);				    \n"
	"	gl_Position *= ProjectionMatrix;						    \n"	
    "   TextureCoordinate = InTextureCoordinate;                    \n"
    "}                                          				    \n";

const GLchar *FragmentSource =
    "precision mediump float;                                               \n"
    "varying vec2 TextureCoordinate;                                        \n"
    "uniform sampler2D TextureSampler;                                      \n"
    "void main()                                                            \n"
	"{											                            \n"
    "   vec4 TextureColor = texture2D(TextureSampler, TextureCoordinate);   \n"
    "   if (TextureColor.a < 0.1)                                           \n"
    "   {                                                                   \n"
    "       discard;                                                        \n"
    "   }                                                                   \n"
    "   gl_FragColor = TextureColor;                                        \n"
    "}                                                                      \n";
