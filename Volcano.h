#ifndef VOLCANO_H
#define VOLCANO_H

#include "ofMain.h"
#include "FrameAnimation.h"
#include "PAnimal.h"


class VDrop{
public:
	float origin_x,origin_y;
	float x,y;
	float dest_y;
	float vel;
	bool onleft;

	VDrop(float x_,float y_,float dest_,bool left_){
		x=x_; y=y_;
		origin_x=x_; origin_y=y_;
		dest_y=dest_;
		vel=1;//ofRandom(.02,.1)*(dest_y-y);
		onleft=left_;
	}
	void update(int mode,int jump_stage,float jump_portion,float cycle_dest){
		if(jump_stage==2 || mode==3){
			y=origin_y+(cycle_dest-origin_y)*abs(jump_portion);			
		}
		
	}
	void reset(){
		x=origin_x; y=origin_y;
	}


};

class Volcano{

	
	ofColor fc;
	float dest_rad;
	float vel;
	float phi;

	vector<ofVec2f> top_vertex;
	vector<ofVec2f> ash_vertex;
	vector<VDrop> ash_drops;

	vector<float> firework_vertex;
	FrameAnimation ani_fire;
	
	int _cur_mode,_dest_mode;
	FrameAnimation ani_change;

	float _dest_height;
public:
	float x,y,wid,hei;


	Volcano(float x_,float y_,float wid_,float hei_,FrameAnimation ani_,float dest_,float TIME_CHANGE){
		x=x_; y=y_; wid=wid_; hei=hei_;
		//vel=vel_; phi=phi_;


		phi=ofRandom(TWO_PI);

		fc=ofColor(ofRandom(50,100)+150,ofRandom(20,40)+120,80);

		int mvert=(int)ofRandom(8,28);
		for(int i=0;i<mvert;++i)
			top_vertex.push_back(ofVec2f(wid/8*5-wid/4/(float)mvert*i,ofRandom(-.3,.3)*hei));
		
		dest_rad=dest_;
		for(int i=0;i<mvert;++i)
			ash_vertex.push_back(ofVec2f(PI/2/(float)mvert*ofRandom(.2,.6),ofRandom(-.5,1.5)*hei));
			// ash_vertex.add(new ofVec2f(wid/8*5-wid/4/(float)mvert*i,ofRandom(-.5,1.5)*hei));
	
		// firework_vertex=new FloatList();
		// float tmp_ang=0;
		// for(int i=0;i<mvert;++i){
		// 	float ang=ofRandom(TWO_PI/(float)mvert);
		// 	ang=constrain(ang,tmp_ang,TWO_PI-tmp_ang);
		// 	firework_vertex.append(ang+tmp_ang);
		// 	tmp_ang+=ang;
		// }

		for(int i=0;i<mvert*5;++i)
			ash_drops.push_back(VDrop(ofRandom(wid/8*3,wid/8*5),ofRandom(-.3,.3)*hei,dest_rad,i>mvert*5/2));

		ani_fire=ani_;
	

		ani_change=FrameAnimation(TIME_CHANGE);

	}
	void reset(){
		ani_fire.Reset();
	}
	void update(){
		ani_fire.Update();
		ani_change.Update();
		if(ani_change.GetPortion()==1) _cur_mode=_dest_mode;
	}
	void changeMode(int mode_,float h_){
		_cur_mode=_dest_mode;
		_dest_mode=mode_;
		_dest_height=h_;
		ani_change.Restart();

		if(mode_==2) ani_fire.Restart();
	}
	void setPP(PAnimal& pa){
		dest_rad=pa.v_jump_dest_y;
		ani_fire=pa._anim_jump;
		
	}
	void draw(bool draw_fill,bool draw_ash,bool draw_fire){

		float draw_portion=ani_fire.GetPortion();
		//println(draw_portion);

		ofPushStyle();
		if(draw_fill){
			//pg.noStroke(); 
			ofSetColor(fc);
		}else{
			ofSetColor(0); ofNoFill();
		}

		ofPushMatrix();
		ofTranslate(x-wid/2,y);

			if(draw_ash)	drawAsh(draw_fill);
			//if(draw_fire)	drawFireworks(pg,draw_fill,draw_portion);
			
			
			ofBeginShape();
				if(draw_fill){
					ofVertex(0,hei);
					ofBezierVertex(wid/3*draw_portion,hei+hei/8*draw_portion,wid/3*2*draw_portion,hei+hei/8*draw_portion,
								wid,hei);
				}else ofVertex(wid,hei);
				ofBezierVertex(wid/8*7,hei/3*2+hei/2*draw_portion,wid/8*6,hei/3+hei/2*draw_portion,
								wid/8*5,-hei/12*draw_portion);
				
				for(ofVec2f v:top_vertex) ofVertex(v.x,v.y+v.y/2*draw_portion);

				ofBezierVertex(wid/8*3,hei/3+hei/4*draw_portion,wid/8,hei/3*2+hei/2*draw_portion,
								0,hei+hei/12*draw_portion);
			ofEndShape();

			
			if(draw_fill){
				// ofSetColor(fc);
				ofSetColor(30,80);
				ofNoFill();
				ofTranslate(wid*sin(phi),hei);
				int mvert=6;//(int)ofRandom(6,16);
				float last_x=0,last_y=0;
				ofBeginShape();
					ofVertex(0,0);
					for(int x=0;x<mvert;++x){
						// float tmp_x=x*wid/8*cos(phi*draw_portion+x)+wid/4*draw_portion;
						// float tmp_y=(hei/12*x)*(1+draw_portion+2*cos(phi));
						float tmp_x=x*wid/8*cos(phi+x)+wid/4;
						float tmp_y=(hei/12*x)*(1+1+2*cos(phi));
						if(x%3<1) 
							ofVertex(tmp_x,tmp_y);
						else ofBezierVertex(tmp_x+wid/8,tmp_y-hei/2*sin(x),last_x-wid/8,last_y-hei/2*sin(x),
											 tmp_x,tmp_y);

						last_x=tmp_x; last_y=tmp_y;
					}	
				ofEndShape();
			}
				

		ofPopMatrix();

		ofPopStyle();

//		update();

	}
	void drawAsh(bool draw_fill){
		
		float jump_portion=ani_fire.GetPortion();
		int jump_stage=ani_fire._loop_count%4;

		//int jump_y_cycle=floor(((float)frameCount/(vel)+phi)/TWO_PI);
		float cycle_dest_y=dest_rad;//(.3+1.2*abs(sin(jump_y_cycle)))*dest_rad;
		if(_dest_mode==3){
			//cycle_dest_y-=_dest_height*.8*constrain(ani_change.GetPortion(),0,.5)*2;			
			cycle_dest_y-=_dest_height*.8*ani_change.GetPortion();			
		} 

		float draw_portion=0;
		if(jump_stage==2||jump_stage==3) draw_portion=abs(jump_portion);
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_SCREEN);
		ofSetLineWidth(1);
		if(draw_fill){
			//pg.noStroke(); 
			ofSetColor(fc.r*2.5,fc.g*.5,0,180); 
			ofNoFill();
			//ofSetLineWidth(ofRandom(2,5));
		}else{
			ofSetColor(20,170); ofNoFill();
		}

			ofPushMatrix();
				ofTranslate(0,hei/2);
				float mash=ash_vertex.size();
				float rad=dest_rad*draw_portion;

				ofVec2f dest;
				for(int i=0;i<mash;++i){
					// float ang=-PI/4+PI/2/mash*i+PI;
					// ofVec2f v=ash_vertex.get(i);
					
					// int mnoise=abs((int)(v.x*100));//(int)ofRandom(4,12);
					// ofBeginShape();

					// 	for(int x=0;x<mnoise;++x){
					// 		float trad=ofRandom(v.y,rad);
					// 		float tang=ofRandom(-v.x,v.x)+ang;
					// 		if(x%3==0) ofVertex(wid/2+trad*sin(tang),trad*cos(tang));
					// 		else ofBezierVertex(wid/2+trad*sin(tang)*draw_portion,trad*cos(tang),
					// 						wid/2+trad*sin(tang),trad*cos(tang)*draw_portion,
					// 						wid/2+trad*sin(tang),trad*cos(tang));
					// 	}

					// ofEndShape();

					VDrop drop=ash_drops[i];

					int mnoise=(int)ofRandom(3,8);

					ofBeginShape();
							float last_tx=0,last_ty=0;
							for(int x=0;x<mnoise;++x){

								float tx,ty;

								if(_dest_mode==3 ||jump_stage==2){
									//tx=drop.x+ofLerp(-wid/12,wid/12,ofNoise(x));
									//ty=ofLerp(-cycle_dest_y/(float)mnoise*x,hei/2,ofNoise(x));
									tx=ofRandom(-wid/12,wid/12)+drop.x;
									ty=ofRandom(-cycle_dest_y/(float)mnoise*x,hei/2);//ofRandom(-drop.y-cycle_dest_y/3,hei/2);
									// if(ty<-cycle_dest_y){
									// 	ty=ty+ofRandom(-.3,.3)*cycle_dest_y;
									// 	tx=ofRandom(-wid/2,wid/2)+drop.x;
									// }
									if(ofRandom(3)<1) ofVertex(tx,ty);
									else ofBezierVertex((last_tx+tx)/2*draw_portion,(last_ty+ty)/2,
														(last_tx+tx)/2,(last_ty+ty)/2*draw_portion,
														tx,ty);
								}else{
									tx=ofRandom(-wid/25,wid/25)+drop.x;
									ty=ofRandom(-hei,0);
									ofVertex(tx,ty);
								}
							
								last_tx=tx;
								last_ty=ty;
							}

						ofEndShape();
					
					// pg.arc(drop.x,-drop.y,10,10,0,PI);

					//drop.update(_dest_mode,jump_stage,jump_portion,cycle_dest_y);
					if(drop.x>wid||drop.x<-wid) drop.reset();
				}

			ofPopMatrix();
		

		ofPopStyle();

	}


};


#endif