in vec4 inVertex;
//out gl_PerVertex { vec4 gl_Position; float gl_PointSize; float gl_ClipDistance[2]; };
out float gl_ClipDistance[2];
uniform mat4 u_ModelView;
uniform mat4 u_Projection;
uniform vec4 u_plane0;
uniform vec4 u_plane1 = vec4(0,-1,0,0);

void main(){
	vec4 vsPos = u_ModelView * inVertex;
	gl_Position = u_Projection * vsPos;
	
	gl_ClipDistance[0] = dot(u_plane0, vsPos);
	gl_ClipDistance[1] = dot(u_plane1, vsPos);
}