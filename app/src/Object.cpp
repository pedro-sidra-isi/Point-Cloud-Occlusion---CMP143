#include "Object.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include <algorithm>
#include <cmath>
#include <memory>

Object::~Object() {}

Object::Object(char const *FileName) : VA(), layout() { this->Load(FileName); }

void Object::Load(char const *FileName) {
  std::vector<Triangle> Tris;
  int material_count = 0, color_index[3], i = 0;
  char line[10];
  char ch;
  FILE *fp = fopen(FileName, "r");

  if (fp == NULL) {
    printf("ERROR: unable to open TriObj [%s]!\n", FileName);
    exit(1);
  }
  fscanf(fp, "%c", &ch);

  while (ch != '\n') // skip the first line – object’s name
    fscanf(fp, "%c", &ch);

  fscanf(fp, "# triangles = %d\n", &NumTris);           // read # of triangles
  fscanf(fp, "Material count = %d\n", &material_count); // read material count
  //
  for (i = 0; i < material_count; i++) {
    Material m;
    fscanf(fp, "ambient color %f %f %f\n", &(m.ambient.x), &(m.ambient.y),
           &(m.ambient.z));
    fscanf(fp, "diffuse color %f %f %f\n", &(m.diffuse.x), &(m.diffuse.y),
           &(m.diffuse.z));
    fscanf(fp, "specular color %f %f %f\n", &(m.specular.x), &(m.specular.y),
           &(m.specular.z));
    fscanf(fp, "material shine %f\n", &(m.shine));
    materials.push_back(m);
  }

  fscanf(fp, "Texture = %s\n", line);

  std::cout << line << std::endl;

  fscanf(fp, "%c", &ch);
  while (ch != '\n') // skip documentation line
    fscanf(fp, "%c", &ch);

  // allocate triangles for tri model

  printf("Reading in %s (%d triangles). . .\n", FileName, NumTris);
  // Triangle *Tris = new Triangle[NumTris];
  Tris.reserve(NumTris);
  //
  for (i = 0; i < NumTris; i++) // read triangles
  {
    Tris.push_back(Triangle());
    float ignore;

    GLfloat *t0, *t1;
    t0 = t1 = &ignore;

    std::string scanf_input_0, scanf_input_1, scanf_input_2;
    if (line[0] == 'Y') {
      scanf_input_0 = "v0 %f %f %f %f %f %f %d %f %f\n";
      scanf_input_1 = "v1 %f %f %f %f %f %f %d %f %f\n";
      scanf_input_2 = "v2 %f %f %f %f %f %f %d %f %f\n";
    } else {
      scanf_input_0 = "v0 %f %f %f %f %f %f %d\n";
      scanf_input_1 = "v1 %f %f %f %f %f %f %d\n";
      scanf_input_2 = "v2 %f %f %f %f %f %f %d\n";
    }
    fscanf(fp, scanf_input_0.c_str(), &(Tris[i].v0.x), &(Tris[i].v0.y),
           &(Tris[i].v0.z), &(Tris[i].normal_0.x), &(Tris[i].normal_0.y),
           &(Tris[i].normal_0.z), &(color_index[0]), t0, t1);
    fscanf(fp, scanf_input_1.c_str(), &(Tris[i].v1.x), &(Tris[i].v1.y),
           &(Tris[i].v1.z), &(Tris[i].normal_1.x), &(Tris[i].normal_1.y),
           &(Tris[i].normal_1.z), &(color_index[1]), t0, t1);
    fscanf(fp, scanf_input_2.c_str(), &(Tris[i].v2.x), &(Tris[i].v2.y),
           &(Tris[i].v2.z), &(Tris[i].normal_2.x), &(Tris[i].normal_2.y),
           &(Tris[i].normal_2.z), &(color_index[2]), t0, t1);
    fscanf(fp, "face normal %f %f %f\n", &(Tris[i].face_normal.x),
           &(Tris[i].face_normal.y), &(Tris[i].face_normal.z));
    //
    Tris[i].material_index_0 = color_index[0];
    Tris[i].material_index_1 = color_index[1];
    Tris[i].material_index_2 = color_index[2];
  }
  fclose(fp);

  vertex_data.reserve(3 * NumTris);
  Vertex v;
  for (i = 0; i < NumTris; i++) {
    // build and push three vertexes per triangle
    v.coordinate[0] = Tris[i].v0.x;
    v.coordinate[1] = Tris[i].v0.y;
    v.coordinate[2] = Tris[i].v0.z;
    v.normal[0] = Tris[i].normal_0.x;
    v.normal[1] = Tris[i].normal_0.y;
    v.normal[2] = Tris[i].normal_0.z;
    v.material_index = Tris[i].material_index_0;
    vertex_data.push_back(Vertex(v));

    v.coordinate[0] = Tris[i].v1.x;
    v.coordinate[1] = Tris[i].v1.y;
    v.coordinate[2] = Tris[i].v1.z;
    v.normal[0] = Tris[i].normal_1.x;
    v.normal[1] = Tris[i].normal_1.y;
    v.normal[2] = Tris[i].normal_1.z;
    v.material_index = Tris[i].material_index_0;
    vertex_data.push_back(Vertex(v));

    v.coordinate[0] = Tris[i].v2.x;
    v.coordinate[1] = Tris[i].v2.y;
    v.coordinate[2] = Tris[i].v2.z;
    v.normal[0] = Tris[i].normal_2.x;
    v.normal[1] = Tris[i].normal_2.y;
    v.normal[2] = Tris[i].normal_2.z;
    v.material_index = Tris[i].material_index_0;
    vertex_data.push_back(Vertex(v));

    // printf("normal=(%2.2f,%2.2f,%2.2f), color=(%2.2f,%2.2f,%2.2f), "
    //        "position=(%2.2f,%2.2f,%2.2f)\n",
    //        v.normal[0], v.normal[1], v.normal[2], v.color[0], v.color[1],
    //        v.color[2], v.coordinate[0], v.coordinate[1], v.coordinate[2]);
  }

  // setup VA
  VB = std::make_unique<VertexBuffer>((GLfloat *)vertex_data.data(),
                                      vertex_data.size() * sizeof(Vertex),
                                      vertex_data.size());

  std::cout << VB->getCount() << " - " << vertex_data.size() << std::endl;

  layout.AddFloat(4);       // coordinates
  layout.AddFloat(3);       // normals
  layout.AddUnsignedInt(1); // Material index
  VA.AddBuffer(*VB, layout);
}

GLuint Object::numTriangles() { return NumTris; }

GLuint Object::numVertices() { return 3 * NumTris; }
