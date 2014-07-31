#include "Renderer.h"
#include "Camera.h"
#include "Clock.h"
#include <QtGui\QKeyEvent>
#include <QtGui\QMouseEvent>
#include <glm\gtx\transform.hpp>
#include <Qt\qfile.h>
#include <Qt\qtextstream.h>
#include <ShapeData.h>
#include <ShapeGenerator.h>
#include <iostream>
#include <Qt\qdebug.h>
#include <noise\noise.h>
#include <stdlib.h>
#include <time.h>

Renderer::Renderer()
{
	dt = 0;

	NextShaderInfoIndex = 0;
	NextGeometryIndex = 0;
	NextRenderableIndex = 0;
	NextVertexBufferIndex = 0;
	NextIndexBufferIndex = 0;
	NextUniformIndex = 0;
	NextTextureIndex = 0;

	CurrentIndexOffset = 0;
	CurrentVertexOffset = 0;
	
	BufferSize = 1048576;

	Framebuffer = 0;

	MainCamera.SetPosition(0,0,4.0f);
	MainCamera.SetTarget(0,0,-1);
}

Renderer::~Renderer()
{

}

void Renderer::initializeGL()
{
	grabKeyboard();
	setFocusPolicy(Qt::StrongFocus);

	glewInit();

	clockManager.initialize();

	glDepthFunc(GL_LESS);

	setMinimumHeight(900);

	GenerateBuffers();
	CreateShadowMap();
	CreateSkyBox();
	CreateNoiseTexture();

	glClearColor(0,0,0,1);

	emit renderInitialized();
}

void Renderer::Update()
{
	clockManager.newFrame();
}

void UniformManager::UniformInformation::UpdateInformation(const uint& DataSize, const void* DataIn)
{
	memcpy(Data, DataIn, DataSize);
}

const uint Renderer::AddGeometry(const void* VerticeData, const uint& VertexBufferSize, const unsigned short* indices, const uint& NumIndices, const uint& IndexMode)
{
	assert(NextGeometryIndex < MAX_NUM_GEOMETRIES);
	GeometryInformation& geometry = Geometries[NextGeometryIndex];

	geometry.IndexBufferOffset = CurrentIndexOffset;
	geometry.IndexBufferSize = (NumIndices * sizeof(ushort));
	geometry.NumIndices = NumIndices;
	geometry.VertexBufferOffset = CurrentVertexOffset;
	geometry.VertexBufferSize = VertexBufferSize;
	geometry.IndexMode = IndexMode;

	AddToOpenVertexBuffer(VerticeData, geometry.VertexBufferSize, geometry);
	AddToOpenIndexBuffer(indices, geometry.IndexBufferSize, geometry);

	CurrentIndexOffset += geometry.IndexBufferSize;
	CurrentVertexOffset += geometry.VertexBufferSize;

	return NextGeometryIndex++;
}

void Renderer::ShutDown()
{
	NextShaderInfoIndex = 0;
	NextGeometryIndex = 0;
	NextRenderableIndex = 0;
	NextVertexBufferIndex = 0;
	NextIndexBufferIndex = 0;
	NextUniformIndex = 0;
	NextTextureIndex = 0;

	CurrentIndexOffset = 0;
	CurrentVertexOffset = 0;

	BufferSize = 1048576;

	MainCamera.SetPosition(0,0,2.0f);

	glDeleteBuffers(1, &VertexBufferID);
	glDeleteBuffers(1, &IndexBufferID);
}

const uint Renderer::AddShader(const char* VertexShaderFileName, const char* FragShaderFileName)
{
	assert(NextShaderInfoIndex < MAX_NUM_SHADERS);
	ShaderInformation& shader = ShadersInfos[NextShaderInfoIndex];

	shader.VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	shader.FragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	shader.GlProgramID = glCreateProgram();

	CompileShader(VertexShaderFileName, shader.VertexShaderID);
	CompileShader(FragShaderFileName, shader.FragShaderID);
	
	glAttachShader(shader.GlProgramID, shader.VertexShaderID);
	glAttachShader(shader.GlProgramID, shader.FragShaderID);

	glLinkProgram(shader.GlProgramID);
	
	return NextShaderInfoIndex++;
}

Renderer::Renderable* Renderer::AddRenderable(const uint& GeomertryId, const glm::mat4& where, const uint& ShaderProgramID, const bool& IgnoreDepth, const bool& UseBlend)
{
	assert(NextRenderableIndex < MAX_NUM_RENDERABLES);
	Renderable& render = Renderables[NextRenderableIndex];

	render.GeometryID = GeomertryId;
	render.where = where;
	render.ShaderProgramID = ShaderProgramID;
	render.Depth = IgnoreDepth;	
	render.Blend = UseBlend;
	NextRenderableIndex++;	

	glm::vec3 Ambient(0.1f,0.1f,0.1f);
	render.AddUniform(Uniform.GLMVEC3, "AmbientLight", SIZE_32, reinterpret_cast<const void*>(&Ambient));
	glm::vec3 LightColor(1,1,1);
	render.AddUniform(Uniform.GLMVEC3, "LightColor", SIZE_32, reinterpret_cast<const void*>(&LightColor));
	
	return &render;
}

UniformManager::UniformInformation* Renderer::Renderable::AddUniform(const UniformManager::UniformType& Format, const char* Name, const uint& DataSize, const void* Data)
{
	assert(NumUniforms < UNIFORMS_MAX_CAPACITY);

	Uniforms[NumUniforms].Initialize(Format, Name, DataSize, Data);

	return &(Uniforms[NumUniforms++]);
}
	
void Renderer::AddToOpenVertexBuffer(const void* VerticeData, uint& VertexBufferSize, const GeometryInformation& GeometryInfo)
{
	assert(NextVertexBufferIndex < MAX_NUM_VERTEXBUFFER_IDS);
	VertexBufferIDs[NextVertexBufferIndex] = VertexBufferID;
	
	uint newVertexBufferID = VertexBufferIDs[NextVertexBufferIndex];

	glBindBuffer(GL_ARRAY_BUFFER, newVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, BufferSize, NULL, GL_STATIC_DRAW);
	
	VertexBufferSize = GeometryInfo.VertexBufferSize;

	glBufferSubData(GL_ARRAY_BUFFER, GeometryInfo.VertexBufferOffset, VertexBufferSize, VerticeData);

	if(CurrentVertexOffset >= BufferSize)
		NextVertexBufferIndex++;
}

void Renderer::AddToOpenIndexBuffer(const unsigned short* Indices, const uint& IndexBufferSize, const GeometryInformation& GeometryInfo)
{
	assert(NextIndexBufferIndex < MAX_NUM_INDEXBUFFER_IDS);
	IndexBufferIDs[NextIndexBufferIndex] = IndexBufferID;

	uint newIndexBufferID = IndexBufferIDs[NextIndexBufferIndex];

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newIndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, BufferSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GeometryInfo.IndexBufferOffset, IndexBufferSize, Indices);

	if(CurrentIndexOffset >= BufferSize)
		NextIndexBufferIndex++;
}

const uint Renderer::AddTexture(const char* TextureName)
{	
	assert(NextTextureIndex < MAX_NUM_TEXTURES);
	TextureInformation& textureInfo = TextureInfos[NextTextureIndex];
		
	textureInfo.image = QGLWidget::convertToGLFormat(QImage(TextureName));
	glGenTextures(1, &textureInfo.TextureID);
	glBindTexture(GL_TEXTURE_2D, textureInfo.TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureInfo.image.width(), textureInfo.image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, textureInfo.image.bits());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return NextTextureIndex++;
}

void Renderer::CompileShader(const char* FileName, GLuint ShaderID)
{
	QFile input(FileName);
	if(!input.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug()<<"File failed to open: "<<FileName;
		exit(1);
	}

	QTextStream stream(&input);
    QString shaderCodeText = stream.readAll();
    input.close();
    std::string temp = shaderCodeText.toStdString();
    const char *shaderSource = temp.c_str();

    const char* buf[1];
    buf[0] = shaderSource;
    glShaderSource(ShaderID,1,buf,NULL);
    glCompileShader(ShaderID);

    GLint status;
    glGetShaderiv(ShaderID,GL_COMPILE_STATUS,&status);

    if(status != GL_TRUE)
    {
        //TODO:
        //Log this
        GLint infoLogLength;
        glGetShaderiv(ShaderID,GL_INFO_LOG_LENGTH,&infoLogLength);
        char* buffer = (char*) malloc(infoLogLength);
        GLsizei bitBucket;
        glGetShaderInfoLog(ShaderID,infoLogLength,&bitBucket,buffer);
        free(buffer);
    }
}

void Renderer::EnableAttribs(const uint& Index, const uint& Size, const void* Pointer)
{	
	glEnableVertexAttribArray(Index);
	glVertexAttribPointer(Index,Size,GL_FLOAT,GL_FALSE,Neumont::Vertex::STRIDE, Pointer);		
}

void Renderer::GenerateBuffers()
{	
	glGenBuffers(1,&VertexBufferID);
	glGenBuffers(1,&IndexBufferID);
	glGenFramebuffers(1, &Framebuffer);
}

void Renderer::DrawObjects(const glm::mat4& Camera, const glm::mat4& Projection)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glm::mat4 viewPerspective(Projection * Camera);

	for(uint i = 0; i < NextRenderableIndex; i++)
	{
		if(!Renderables[i].Visible)
			continue;

		if(Renderables[i].Blend)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
			glEnable(GL_CULL_FACE);
		}

		else
		{
			glDisable(GL_BLEND);
			glDisable(GL_CULL_FACE);
		}

		if(Renderables[i].Depth)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		ShaderInformation& shader = ShadersInfos[Renderables[i].ShaderProgramID];
		GeometryInformation& geometry = Geometries[Renderables[i].GeometryID];
		//TextureInformation& texture = TextureInfos[Renderables[i].Texture];
		UniformManager::UniformInformation* uniforms = Renderables[i].Uniforms;

		glUseProgram(shader.GlProgramID);			
			
		GLint uniformLocation;			

		for (uint j = 0; j < Renderables[i].NumUniforms; j++)
		{
			uniformLocation = glGetUniformLocation(shader.GlProgramID, uniforms[j].Name);

			if(uniformLocation >= 0)
				Uniform.MatchUniform(uniformLocation, uniforms[j], viewPerspective);
		}
		
		// Compute the MVP matrix from the light's point of view
		glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
		glm::mat4 depthViewMatrix = glm::lookAt(LightPosition, glm::vec3(0,0,0), glm::vec3(0,1,0));
		glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * Renderables[i].where;	

		glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
		);
		glm::mat4 depthBiasMVP = biasMatrix * depthMVP;

		uniformLocation = glGetUniformLocation(shader.GlProgramID, "mvp");
		glm::mat4 mvp = viewPerspective * Renderables[i].where;
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE,&mvp[0][0]);
					
		uniformLocation = glGetUniformLocation(shader.GlProgramID, "DepthBiasMVP");
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &depthBiasMVP[0][0]);
			
		uniformLocation = glGetUniformLocation(shader.GlProgramID, "LightPosition");		
		glUniform3f(uniformLocation, LightPosition.x,LightPosition.y,LightPosition.z);
		
		uniformLocation = glGetUniformLocation(shader.GlProgramID, "EyeDirection");
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &MainCamera.GetView()[0][0]);	

		uniformLocation = glGetUniformLocation(shader.GlProgramID, "ModelToWorldMatrix");
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &Renderables[i].where[0][0]);	
				
		uniformLocation = glGetUniformLocation(shader.GlProgramID, "LowThreshold");
		glUniform1fv(uniformLocation, 1, &LowThreshold);

		uniformLocation = glGetUniformLocation(shader.GlProgramID, "HighThreshold");
		glUniform1fv(uniformLocation, 1, &HighThreshold);

		uniformLocation = glGetUniformLocation(shader.GlProgramID, "Time");
		dt += clockManager.timeElapsedLastFrame();
		glUniform1fv(uniformLocation, 1, &dt);

		uniformLocation = glGetUniformLocation(shader.GlProgramID, "K");
		glUniform1fv(uniformLocation, 1, &K);

		uniformLocation = glGetUniformLocation(shader.GlProgramID, "Velocity");
		glUniform1fv(uniformLocation, 1, &Velocity);

		uniformLocation = glGetUniformLocation(shader.GlProgramID, "Amplitude");
		float amplitude = 0.1f;
		glUniform1fv(uniformLocation, 1, &amplitude);
				
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		uniformLocation = glGetUniformLocation(shader.GlProgramID, "shadowMap");
		glUniform1i(uniformLocation, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, NoiseTexture);
		uniformLocation = glGetUniformLocation(shader.GlProgramID, "NoiseTexture");
		glUniform1i(uniformLocation, 1);

		glActiveTexture(GL_TEXTURE2);
		if(NormalsOn)
			glBindTexture(GL_TEXTURE_2D, 4);
		else
			glBindTexture(GL_TEXTURE_2D, 10);
		uniformLocation = glGetUniformLocation(shader.GlProgramID, "NormalTexture");
		glUniform1i(uniformLocation, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, Renderables[i].Texture);
		uniformLocation = glGetUniformLocation(shader.GlProgramID, "defaultTexture");
		glUniform1i(uniformLocation, 3);
						
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapTexture);
		uniformLocation = glGetUniformLocation(shader.GlProgramID, "gCubemapTexture");
		glUniform1i(uniformLocation, 4);

		GLuint Index;

		if(LightsOn)
			Index = glGetSubroutineIndex(shader.GlProgramID, GL_FRAGMENT_SHADER, "enableLights");
		else
			Index = glGetSubroutineIndex(shader.GlProgramID, GL_FRAGMENT_SHADER, "disableLights");

		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &Index);
												
		EnableAttribs(0,3, BUFFER_OFFSET(geometry.VertexBufferOffset + Neumont::Vertex::POSITION_OFFSET));
		EnableAttribs(1,4, BUFFER_OFFSET(geometry.VertexBufferOffset + Neumont::Vertex::COLOR_OFFSET));
		EnableAttribs(2,2, BUFFER_OFFSET(geometry.VertexBufferOffset + Neumont::Vertex::UV_OFFSET));		
		EnableAttribs(3,3, BUFFER_OFFSET(geometry.VertexBufferOffset + Neumont::Vertex::NORMAL_OFFSET));
		EnableAttribs(4,3, BUFFER_OFFSET(geometry.VertexBufferOffset + Neumont::Vertex::TANGENT_OFFSET));
		EnableAttribs(5,3, BUFFER_OFFSET(geometry.VertexBufferOffset + Neumont::Vertex::BITANGENT_OFFSET));
				
		glDrawElements(geometry.IndexMode, geometry.NumIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(geometry.IndexBufferOffset));
				
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);
        glDisableVertexAttribArray(5);
	}
}	

void Renderer::DrawShadows(const glm::mat4& Camera, const glm::mat4& Projection)
{	
	// -1 because plane is not on the scene, plane = floor, round, terrain, etc.

	for(uint i = 1; i < NextRenderableIndex - 1; i++)
	{
		if(!Renderables[i].Visible)
			continue;

		GeometryInformation& geometry = Geometries[Renderables[i].GeometryID];

		glUseProgram(ShadersInfos[0].GlProgramID);				
			
		GLint uniformLocation;					
		uniformLocation = glGetUniformLocation(ShadersInfos[0].GlProgramID, "mvp");
		glm::mat4 depthMVP = Projection * Camera * Renderables[i].where;
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &depthMVP[0][0]);
		
		EnableAttribs(0,3, BUFFER_OFFSET(geometry.VertexBufferOffset + Neumont::Vertex::POSITION_OFFSET));

		glDrawElements(geometry.IndexMode, geometry.NumIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(geometry.IndexBufferOffset));
		
        glDisableVertexAttribArray(0);
	}
}

void Renderer::paintGL()
{
	MainCamera.UpdateView();

	glViewport(0,0,width(),height());
		
	//Compute the MVP matrix from the light's point of view
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
	glm::mat4 depthViewMatrix = glm::lookAt(LightPosition, glm::vec3(0,0,0), glm::vec3(0,1,0));
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(ShadowsOn)
		DrawShadows(depthViewMatrix, depthProjectionMatrix);	

	//screen framebuffer
	glm::mat4 camera =  MainCamera.GetView();	
	Projection = glm::perspective(60.0f,(float)width()/height(), 0.1f,100.0f);
	DrawObjects(camera, Projection);

	//display shadowMap
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	
	glViewport(0,-20,width()/6,height()/4);
	if(ShadowsOn)
		DrawShadows(depthViewMatrix, depthProjectionMatrix);
}

void Renderer::CreateSkyBox()
{
	QImage texture_data[6];

	glGenTextures(1, &CubeMapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapTexture);
				
	texture_data[0] = QGLWidget::convertToGLFormat(QImage("back.png"));
	texture_data[1] = QGLWidget::convertToGLFormat(QImage("front.png"));
	texture_data[2] = QGLWidget::convertToGLFormat(QImage("bottom.png"));
	texture_data[3] = QGLWidget::convertToGLFormat(QImage("top.png"));
	texture_data[4] = QGLWidget::convertToGLFormat(QImage("left.png"));
	texture_data[5] = QGLWidget::convertToGLFormat(QImage("right.png"));
	
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 10, GL_RGBA8, texture_data[0].width(), texture_data[0].height());

	for (int face= 0; face < 6; face++)
	{
		GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
		glTexSubImage2D(target, 0, 0, 0, texture_data[face].width(), texture_data[face].height(), GL_RGBA, GL_UNSIGNED_BYTE, texture_data[face].bits());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Renderer::CreateShadowMap()
{			
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, width(), height(), 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0); 
	glDrawBuffer(GL_NONE);
		
	glBindTexture(GL_TEXTURE_2D, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		qDebug()<<glCheckFramebufferStatus(GL_FRAMEBUFFER);
}

void Renderer::ScreenCapture()
{
	unsigned char* buffer = (unsigned char*)malloc (width() * height() * 3);
	glReadPixels(0, 0, width(), height(), GL_RGB, GL_UNSIGNED_BYTE, buffer);

	char name[1024];
	long int t = time(NULL);
	sprintf(name, "screenshot_%ld.raw", t);

	FILE* fp = fopen(name, "wb");
	if(!fp)
	{
		fprintf(stderr, "ERROR: could not open %s for writting\n", name);
		return;
	}

	int bytes_in_row = width() * 3;
	int bytes_left = width() * height() * 3;
	while (bytes_left > 0)
	{
		int start_of_row = bytes_left - bytes_in_row;
		fwrite(&buffer[start_of_row], 1, bytes_in_row, fp);
		bytes_left -= bytes_in_row;
	}

	fclose(fp);
	free(buffer);
}

void Renderer::CreateNoiseTexture()
{
	int width = 128;
	int height = 128;

	noise::module::Perlin perlinNoise;

	perlinNoise.SetFrequency(4.0);

	GLubyte *data = new GLubyte[width * height * 4];

	float xRange = 1.0;
	float yRange = 1.0;
	float xFactor = xRange / width;
	float yFactor = yRange / height; 

	for (int oct = 0; oct < 4; oct++)
	{
		perlinNoise.SetOctaveCount(oct+1);
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				float x = xFactor * i;
				float y = yFactor * j;
				float z = 0.0;

				float val = 0.0f;
				double a, b, c, d;
				a = perlinNoise.GetValue(x ,y ,z);
				b = perlinNoise.GetValue(x+xRange,y ,z);
				c = perlinNoise.GetValue(x ,y+yRange,z);
				d = perlinNoise.GetValue(x+xRange,y+yRange,z);

				float xmix = 1.0 - x / xRange;
				float ymix = 1.0 - y / yRange;
				float x1 = glm::mix( a, b, xmix );
				float x2 = glm::mix( c, d, xmix );

				val = glm::mix(x1, x2, ymix );

				val = (val + 1.0f) * 0.5f;

				val = val> 1.0 ? 1.0 :val;
				val = val< 0.0 ? 0.0 :val;

				data[((j * width + i) * 4) + oct] = (GLubyte)(val * 255.0f);
			}
		}
	}

	glGenBuffers(1, &NoiseTexture);
	glBindTexture(GL_TEXTURE_2D, NoiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
	glBindTexture(GL_TEXTURE_2D, 0);

	delete [] data;
}

void Renderer::resizeEvent()
{
	glViewport(0,0,width(),height());
}

void Renderer::mouseMoveEvent(QMouseEvent* e)
{
	glm::vec2 currentPos = glm::vec2(e->x(), e->y());

	if(glm::distance(currentPos, lastMousePos) < 100)
	{
		MainCamera.Rotate_Horizontal(0.4f * (currentPos.x - lastMousePos.x));
		MainCamera.Rotate_Vertical(0.4f * (currentPos.y - lastMousePos.y));
	}

	lastMousePos = currentPos;	
	glDraw();
}

void Renderer::keyPressEvent(QKeyEvent* e)
{
	MainCamera.KeyPressedEvent(e);
	switch (e->key())
	{
		case Qt::Key_P:
			ScreenCapture();
			break;
		default:
			e->ignore();
			break;
	}
	repaint();
}