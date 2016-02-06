#ifndef DISTORT_GRAPH_H
#define DISTORT_GRAPH_H

#include "ofMain.h"

class DistortGraph{

	vector<ofVec2f> texturePoints;
	vector<ofVec2f> vertexPoints;
	vector<float> phases;

	
public:
	int play_vel;
	int transform_vel;
	int WID_SEG;
	int HEI_SEG;
	float transform_mag;
	float margin;
	ofMesh _mesh;

	DistortGraph(){
		WID_SEG=20;
		play_vel=5;
		transform_vel=16;
		transform_mag=1.4;
		margin=0;
	}
	DistortGraph(int wid_seg){
		
		WID_SEG=wid_seg;
		play_vel=5;
		transform_vel=16;
		transform_mag=1.8;
		margin=0;

		float w=ofGetWidth()/(float)WID_SEG;
		float hei=ofGetHeight();
		
		HEI_SEG=ceil(hei/w);

		for(int i=0;i<=WID_SEG;++i)
			for(int j=0;j<=HEI_SEG;++j){

			//#ifdef TARGET_OPENGLES
				texturePoints.push_back(ofVec2f((float)i/(float)WID_SEG,(float)j*w/hei));
			//#else
				//texturePoints.push_back(ofVec2f(i*w,j*w));
			//#endif
				vertexPoints.push_back(ofVec2f(i*w,j*w));

				phases.push_back(ofRandom(TWO_PI));
			}
		WID_SEG+=1;
		HEI_SEG+=1;
	}

	float getMargin(){
		return margin;
	}
	float getWeight(){
		return 1+margin*2;
	}
	void updateMesh(){
		// int len=vertexPoints.size();
		
		
		float w=ofGetWidth()*(1+2*margin)/WID_SEG;
		
		_mesh.clear();
		//_mesh.enableTextures();
		//_mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
				

		for(int i=0;i<WID_SEG-1;++i)
			for(int j=0;j<HEI_SEG-1;++j){	
				ofVec2f p1=vertexPoints[i*HEI_SEG+j];
				ofVec2f p2=vertexPoints[(i+1)*HEI_SEG+j];
				ofVec2f p3=vertexPoints[(i+1)*HEI_SEG+j+1];
				ofVec2f p4=vertexPoints[i*HEI_SEG+j+1];
				
				ofVec2f t1=texturePoints[i*HEI_SEG+j];
				ofVec2f t2=texturePoints[(i+1)*HEI_SEG+j];
				ofVec2f t3=texturePoints[(i+1)*HEI_SEG+j+1];
				ofVec2f t4=texturePoints[i*HEI_SEG+j+1];
				
				
				_mesh.addVertex(p1);
				_mesh.addVertex(p2);
				_mesh.addVertex(p3);

				_mesh.addVertex(p3);
				_mesh.addVertex(p4);
				_mesh.addVertex(p1);
				
				_mesh.addTexCoord(t1);
				_mesh.addTexCoord(t2);
				_mesh.addTexCoord(t3);
				
				_mesh.addTexCoord(t3);
				_mesh.addTexCoord(t4);
				_mesh.addTexCoord(t1);
				
				

			}
		/*_mesh.addVertex(ofVec2f(0,0));
		_mesh.addTexCoord(ofVec2f(0,0));
		_mesh.addVertex(ofVec2f(800,0));
		_mesh.addTexCoord(ofVec2f(800,0));
		_mesh.addVertex(ofVec2f(800,480));
		_mesh.addTexCoord(ofVec2f(800,480));*/

		/*ofPushMatrix();
		ofTranslate(-ofGetWidth()*margin,-ofGetHeight()*margin);
		fbo_.getTextureReference().bind();	
		
		_mesh.draw();	
				
		ofPopMatrix();
		fbo_.getTextureReference().unbind();

		update();*/
	}
	void update(){

		int len=texturePoints.size();
		int frameCount=ofGetFrameNum();
		if(frameCount%play_vel==0){
			for(int i=0;i<len;++i){
				vertexPoints[i].x+=sin(frameCount%transform_vel*TWO_PI/transform_vel+phases[i])*transform_mag;
				vertexPoints[i].y+=cos(frameCount%transform_vel*TWO_PI/transform_vel+phases[i])*transform_mag;	
			}
		}

		updateMesh();
	}	
	void reset(){
		int len=vertexPoints.size();
		for(int i=0;i<len;++i){
			ofVec2f vp=vertexPoints[i];
			texturePoints[i].x=vp.x; texturePoints[i].y=vp.y;
		}
	}



};


#endif
