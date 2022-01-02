#include "QuadRenderer.h"

using namespace std;

QuadRenderer::QuadRenderer()
{
}

QuadRenderer::~QuadRenderer()
{
}

void QuadRenderer::initialize(const string& _shaderFolderPath)
{
	float vertex[12] =
	{
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};

	unsigned char color[12] =
	{
		255, 0, 0,
		0, 255, 0,
		0, 0, 255,
		255, 255, 255
	};

	float uv[8] =
	{
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
	};
	
	shader.init(_shaderFolderPath+"/texturedQuadShader.vert", _shaderFolderPath+"/texturedQuadShader.frag");

	vertexHandle = glGetAttribLocation(shader.getShaderId(), "vertexModel");
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertex, GL_STATIC_DRAW);

	uvHandle = glGetAttribLocation(shader.getShaderId(), "vertexUV");
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), uv, GL_STATIC_DRAW);
	
	MVPMatrixHandle = glGetUniformLocation(shader.getShaderId(), "MVPMatrix");

	glGenTextures(1, &textureBuffer);
	glBindTexture(GL_TEXTURE_2D, textureBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 2, 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, color);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	textureHandle = glGetUniformLocation(shader.getShaderId(), "texture");

	modelMatrix.setIdentity();

	shader.bind();
}

void QuadRenderer::onDraw(Eigen::Matrix4f vpMatrix)
{
	shader.bind();

	MVPMatrix = vpMatrix * modelMatrix;

	glUniformMatrix4fv(MVPMatrixHandle, 1, GL_FALSE, MVPMatrix.data());

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(vertexHandle, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(vertexHandle);

	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(uvHandle, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(uvHandle);

	glUniform1i(textureHandle, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureBuffer);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	shader.unbind();

}

void QuadRenderer::setModelMatrix(Eigen::Matrix4f matrix) {
	modelMatrix = matrix;
}

void QuadRenderer::setCameraImage(cv::Mat& inputImage) {
	shader.bind();

	cv::Mat flipImage;// = inputImage;
	cv::flip(inputImage, flipImage, 0);

	int imageWidth = flipImage.cols;
	int imageHeight = flipImage.rows;

	int newImageWidth = 2;
	while (imageWidth > newImageWidth) {
		newImageWidth = newImageWidth << 1;
	}

	cv::Mat newflipImage = cv::Mat(imageHeight, newImageWidth, flipImage.type());
	flipImage.copyTo(newflipImage(cv::Rect(0, 0, imageWidth, imageHeight)));

	float uv[8] =
	{
		0.0f, 1.0f,
		(float)imageWidth / (float)newImageWidth, 1.0f,
		0.0f, 0.0f,
		(float)imageWidth / (float)newImageWidth, 0.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), uv, GL_DYNAMIC_DRAW);

	imageWidth = newImageWidth;

	flipImage = newflipImage;

	glBindTexture(GL_TEXTURE_2D, textureBuffer);
	if (flipImage.channels() == 1) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, flipImage.data);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, flipImage.data);
	}

	shader.unbind();
}

void QuadRenderer::setTexture(GLuint texture) {
	textureBuffer = texture;
}