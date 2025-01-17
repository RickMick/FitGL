#include <BaseApp.h>
#include <map>

#include <geGL/StaticCalls.h>
using namespace ge::gl;
using namespace fgl;

std::string vert = R".(
#version 430
out flat int vid;
void main() {
  vid = gl_VertexID;
	gl_Position = vec4(0,0,0,1);
}).";

std::string tesc = R".(
#version 430
layout(vertices=1)out;
uniform float i[2];
uniform float o[4];
in flat int vid[];
out patch int tid;
void main() {
  tid=vid[0];
  gl_TessLevelInner[0]=64;
  gl_TessLevelInner[1]=64;
  gl_TessLevelOuter[0]=64;
  gl_TessLevelOuter[1]=64;
  gl_TessLevelOuter[2]=64;
  gl_TessLevelOuter[3]=64;
}).";

std::string tese = R".(
#version 430
layout(quads,equal_spacing)in;
uniform vec2 center;

uniform mat4 v;
uniform mat4 p;
uniform int levels;
uniform float weights[8];

in patch int tid;

out vec3 pos;
out vec3 normal;

/* simplex - float
Copyright (C) 2011 by Ashima Arts
MIT Licence
float snoise(vecN x)*/
float mod289(float x){return x-floor(x*(1.0/289.0))*289.0;}vec2 mod289(vec2 x){return x-floor(x*(1.0/289.0))*289.0;}vec3 mod289(vec3 x){return x-floor(x*(1.0/289.0))*289.0;}vec4 mod289(vec4 x){return x-floor(x*(1.0/289.0))*289.0;}float permute(float x){return mod289(((x*34.0)+1.0)*x);}vec2 permute(vec2 x){return mod289(((x*34.0)+1.0)*x);}vec3 permute(vec3 x){return mod289(((x*34.0)+1.0)*x);}vec4 permute(vec4 x){return mod289(((x*34.0)+1.0)*x);}float taylorInvSqrt(float r){return 1.79284291400159-0.85373472095314*r;}vec2 taylorInvSqrt(vec2 r){return 1.79284291400159-0.85373472095314*r;}vec3 taylorInvSqrt(vec3 r){return 1.79284291400159-0.85373472095314*r;}vec4 taylorInvSqrt(vec4 r){return 1.79284291400159-0.85373472095314*r;}float snoise(vec2 v){const vec4 C=vec4(0.211324865405187,0.366025403784439,-0.577350269189626,0.024390243902439);vec2 i=floor(v+dot(v,C.yy));vec2 x0=v-i+dot(i,C.xx);vec2 i1;i1=(x0.x>x0.y)?vec2(1.0,0.0):vec2(0.0,1.0);vec4 x12=x0.xyxy+C.xxzz;x12.xy-=i1;i=mod289(i);vec3 p=permute(permute(i.y+vec3(0.0,i1.y,1.0))+i.x+vec3(0.0,i1.x,1.0));vec3 m=max(0.5-vec3(dot(x0,x0),dot(x12.xy,x12.xy),dot(x12.zw,x12.zw)),0.0);m=m*m;m=m*m;vec3 x=2.0*fract(p*C.www)-1.0;vec3 h=abs(x)-0.5;vec3 ox=floor(x+0.5);vec3 a0=x-ox;m*=1.79284291400159-0.85373472095314*(a0*a0+h*h);vec3 g;g.x=a0.x*x0.x+h.x*x0.y;g.yz=a0.yz*x12.xz+h.yz*x12.yw;return 130.0*dot(m,g);}float snoise(vec3 v){const vec2 C=vec2(1.0/6.0,1.0/3.0);const vec4 D=vec4(0.0,0.5,1.0,2.0);vec3 i=floor(v+dot(v,C.yyy));vec3 x0=v-i+dot(i,C.xxx);vec3 g=step(x0.yzx,x0.xyz);vec3 l=1.0-g;vec3 i1=min(g.xyz,l.zxy);vec3 i2=max(g.xyz,l.zxy);vec3 x1=x0-i1+C.xxx;vec3 x2=x0-i2+C.yyy;vec3 x3=x0-D.yyy;i=mod289(i);vec4 p=permute(permute(permute(i.z+vec4(0.0,i1.z,i2.z,1.0))+i.y+vec4(0.0,i1.y,i2.y,1.0))+i.x+vec4(0.0,i1.x,i2.x,1.0));float n_=0.142857142857;vec3 ns=n_*D.wyz-D.xzx;vec4 j=p-49.0*floor(p*ns.z*ns.z);vec4 x_=floor(j*ns.z);vec4 y_=floor(j-7.0*x_);vec4 x=x_*ns.x+ns.yyyy;vec4 y=y_*ns.x+ns.yyyy;vec4 h=1.0-abs(x)-abs(y);vec4 b0=vec4(x.xy,y.xy);vec4 b1=vec4(x.zw,y.zw);vec4 s0=floor(b0)*2.0+1.0;vec4 s1=floor(b1)*2.0+1.0;vec4 sh=-step(h,vec4(0.0));vec4 a0=b0.xzyw+s0.xzyw*sh.xxyy;vec4 a1=b1.xzyw+s1.xzyw*sh.zzww;vec3 p0=vec3(a0.xy,h.x);vec3 p1=vec3(a0.zw,h.y);vec3 p2=vec3(a1.xy,h.z);vec3 p3=vec3(a1.zw,h.w);vec4 norm=taylorInvSqrt(vec4(dot(p0,p0),dot(p1,p1),dot(p2,p2),dot(p3,p3)));p0*=norm.x;p1*=norm.y;p2*=norm.z;p3*=norm.w;vec4 m=max(0.6-vec4(dot(x0,x0),dot(x1,x1),dot(x2,x2),dot(x3,x3)),0.0);m=m*m;return 42.0*dot(m*m,vec4(dot(p0,x0),dot(p1,x1),dot(p2,x2),dot(p3,x3)));}vec4 grad4(float j,vec4 ip){const vec4 ones=vec4(1.0,1.0,1.0,-1.0);vec4 p,s;p.xyz=floor(fract(vec3(j)*ip.xyz)*7.0)*ip.z-1.0;p.w=1.5-dot(abs(p.xyz),ones.xyz);s=vec4(lessThan(p,vec4(0.0)));p.xyz=p.xyz+(s.xyz*2.0-1.0)*s.www;return p;}float snoise(vec4 v){const vec4 C=vec4(0.138196601125011,0.276393202250021,0.414589803375032,-0.447213595499958);vec4 i=floor(v+dot(v,vec4(0.309016994374947451)));vec4 x0=v-i+dot(i,C.xxxx);vec4 i0;vec3 isX=step(x0.yzw,x0.xxx);vec3 isYZ=step(x0.zww,x0.yyz);i0.x=isX.x+isX.y+isX.z;i0.yzw=1.0-isX;i0.y+=isYZ.x+isYZ.y;i0.zw+=1.0-isYZ.xy;i0.z+=isYZ.z;i0.w+=1.0-isYZ.z;vec4 i3=clamp(i0,0.0,1.0);vec4 i2=clamp(i0-1.0,0.0,1.0);vec4 i1=clamp(i0-2.0,0.0,1.0);vec4 x1=x0-i1+C.xxxx;vec4 x2=x0-i2+C.yyyy;vec4 x3=x0-i3+C.zzzz;vec4 x4=x0+C.wwww;i=mod289(i);float j0=permute(permute(permute(permute(i.w)+i.z)+i.y)+i.x);vec4 j1=permute(permute(permute(permute(i.w+vec4(i1.w,i2.w,i3.w,1.0))+i.z+vec4(i1.z,i2.z,i3.z,1.0))+i.y+vec4(i1.y,i2.y,i3.y,1.0))+i.x+vec4(i1.x,i2.x,i3.x,1.0));vec4 ip=vec4(1.0/294.0,1.0/49.0,1.0/7.0,0.0);vec4 p0=grad4(j0,ip);vec4 p1=grad4(j1.x,ip);vec4 p2=grad4(j1.y,ip);vec4 p3=grad4(j1.z,ip);vec4 p4=grad4(j1.w,ip);vec4 norm=taylorInvSqrt(vec4(dot(p0,p0),dot(p1,p1),dot(p2,p2),dot(p3,p3)));p0*=norm.x;p1*=norm.y;p2*=norm.z;p3*=norm.w;p4*=taylorInvSqrt(dot(p4,p4));vec3 m0=max(0.6-vec3(dot(x0,x0),dot(x1,x1),dot(x2,x2)),0.0);vec2 m1=max(0.6-vec2(dot(x3,x3),dot(x4,x4)),0.0);m0=m0*m0;m1=m1*m1;return 49.0*(dot(m0*m0,vec3(dot(p0,x0),dot(p1,x1),dot(p2,x2)))+dot(m1*m1,vec2(dot(p3,x3),dot(p4,x4))));}


float getHeight(vec2 p){
  // noise or could use texture...

  float div=0;
  float sum=0;
  float a=1;
  float d=0.2;
  for(int i=0;i<levels;i++){
      sum+= weights[i]*a * snoise(p * d);
      div+=a;
      a*=0.5;
      d*=2;
  }
  return sum/div;
}


void main() {  
  vec2 offset = vec2(tid%16-8,tid/16-8);
  vec2 tc = center+offset+gl_TessCoord.xy;
  float height = getHeight(tc);
  pos = vec3(tc.x,height,tc.y);
  
  float e = 1/64.0;;
  vec3 posX = vec3(tc.x+e,getHeight(tc+vec2(e,0)),tc.y);
  vec3 posY = vec3(tc.x,getHeight(tc+vec2(0,e)),tc.y+e);
  normal = normalize(-cross(pos-posX,pos-posY));

  gl_Position = p*v*vec4(pos,1);
}).";

std::string frag = R".(
#version 430
in vec3 pos;
in vec3 normal;
out vec4 fragColor;

vec3 lambert(vec3 pos, vec3 norm, vec3 color,
	vec3 lightPos, vec3 ambientLight, vec3 diffuseLight){
	vec3 N = normalize(norm);
	vec3 L = normalize(lightPos-pos);
	vec3 A = color*ambientLight;
	vec3 D = color*diffuseLight*clamp(dot(N,L),0,1);
	return A+D;
}

void main() {
  vec3 color = vec3(1);
	fragColor = vec4(lambert(pos,normal,color,vec3(5000,5000,500),vec3(0.6),vec3(0.4)),1);
}).";



int main(int /*argc*/, char ** /*argv*/) {
  BaseApp app;
  GLuint vao;
  ProgramS program;

  PerspectiveCameraS cam = newPerspectiveCamera();
  OrbitManipulator manipulator(cam);
  manipulator.setupCallbacks(app);
  manipulator.setZoom(1);

  int levels = 8;
  float weights[8];
  std::fill(weights, weights + 8, 1.f);

  bool wireframe = false;

  app.addInitCallback([&]() {
    auto vs = compileShader(GL_VERTEX_SHADER, vert);
    auto tcs = compileShader(GL_TESS_CONTROL_SHADER, tesc);
    auto tes = compileShader(GL_TESS_EVALUATION_SHADER, tese);
    auto fs = compileShader(GL_FRAGMENT_SHADER, frag);
    program = createProgram(vs, tcs, tes, fs);

    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
  });

  app.addResizeCallback([&](int w, int h) {
    glViewport(0, 0, w, h);
  });
  
  app.addDrawCallback([&]() {
    glClearColor(0.2, 0.2, 0.2, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    program->use();
    program->setMatrix4fv("p", value_ptr(cam->getProjection()));
    program->setMatrix4fv("v", value_ptr(cam->getView()));
    program->set2f("center", cam->getCenter().x, cam->getCenter().z);
    program->set1i("levels", levels);
    for (int i = 0; i < 8; i++) {
      program->set1fv("weights", weights, 8);
    }

    
    glPolygonMode(GL_FRONT_AND_BACK, wireframe?GL_LINE:GL_FILL);

    glPatchParameteri(GL_PATCH_VERTICES, 1 );
    glDrawArrays(GL_PATCHES, 0, 256);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    fpsLabel();
    ImGui::Begin("Options", 0, 0);
    ImGui::PushItemWidth(-80);
    ImGui::Checkbox("Wireframe", &wireframe);
    ImGui::SliderInt("Levels", &levels, 1, 8);
    for (int i = 0; i < levels; i++) {
      std::string label = "weights[" + std::to_string(i) + "]";
      ImGui::SliderFloat(label.c_str(), &weights[i], 0.0f, 10.f);
    }
    ImGui::End();
  });
  return app.run();
}
