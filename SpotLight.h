#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "ofMain.h"
#include "FrameAnimation.h"

class SpotLight{
	
	float x,y,rad,ang;
	float start_ang,end_ang;
	
	FrameAnimation _anim_show;
	FrameAnimation _anim_move;

	ofColor light_ofColor;
public:
	SpotLight(float x_,float y_,float rad_,float ang_,float start_,float end_,float due,float delay,float delay2){
		x=x_; y=y_; rad=rad_; ang=ang_;
		start_ang=start_; end_ang=end_;

		_anim_show=FrameAnimation(due,delay);
		_anim_move=FrameAnimation(ofRandom(20,40),delay2+ofRandom(50));
		_anim_move.is_loop=true;
		resetColor();
	}
	void update(){
		_anim_move.Update();
		_anim_show.Update();
	}
	void restart(){
		resetColor();
		_anim_move.Restart();
		_anim_show.Restart();
	}
	void reset(){
		_anim_move.Reset();
		_anim_show.Reset();	
	}
	void draw(){

		//update();

		ofPushStyle();
		//pg.noStroke();
		ofSetColor(light_ofColor);

		ofPushMatrix();
		ofTranslate(x,y);

		//println(_anim_move.GetPortion());

		float a=ofLerp(start_ang,end_ang,_anim_move.GetPortion());
		ofRotate(a*RAD_TO_DEG);

		ofBeginShape();
		ofVertex(0,0);
		
		ofVec2f v(rad*_anim_show.GetPortion(),0);
		v.rotate(-ang/2*RAD_TO_DEG);
		ofVertex(v.x,v.y);

		v.rotate(ang*RAD_TO_DEG);
		ofVertex(v.x,v.y);

		ofEndShape();

		ofPopMatrix();
		
		ofPopStyle();

	}

	void resetColor(){
		switch((int)ofRandom(3)){
			case 0:
				light_ofColor=ofColor(200+ofRandom(50),50+ofRandom(50),60+ofRandom(50),120);
				break;
			case 1:
				light_ofColor=ofColor(200+ofRandom(50),200+ofRandom(50),ofRandom(50),120);
				break;
			case 2:
				light_ofColor=ofColor(ofRandom(50),200+ofRandom(50),ofRandom(50),120);
				break;
				
		}
	}
};




#endif
