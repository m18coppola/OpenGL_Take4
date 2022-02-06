typedef struct {
	float *verts;
	int verts_size;
	float *tcs;
	int tcs_size;
	float *normals;
	int normals_size;
	unsigned int *v_index;
	int vi_size;
} Model;

char* readShaderSource(char* shaderPath);
GLuint InitShader(char* vShaderPath, char* fShaderPath);
Model *readOBJ(char* OBJpath);
void destroyModel(Model **model_ptr);
