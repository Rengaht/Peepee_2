#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "ofMain.h"
#include "FrameAnimation.h"
#include "PAnimal.h"

class SpaceShip{
	
			
	float phi;
	int mwindow;
	vector<float> window_portion;
	int mray;
	vector<float> ray_portion;
	float vel;
	
	float ray_bottom_y; 
	
	float rot_vel;
	float ray_wid;
	float ray_animal_y;
	float ray_cur_y;

	float ship_delta_x,ship_delta_y;
	float ship_far_pos;

	//int stage=0;
	int start_frame;
	
	float _ray_dest_y,_ray_animal_hei,_ray_animal_wid;
	float _ray_dest_x;
	float _src_y;
	

	float _mode_height;

	float fly_dest;

	int src_mode;
	int dest_mode;
	FrameAnimation _anim_detect;
	
	FrameAnimation _anim_fly;
	FrameAnimation _anim_hide;

	
	ofVec2f _calib_vel;
public:

	float x,y,wid,hei;
	float dx,dy;
	FrameAnimation _anim_catch;
	ofVec2f volc_dest;

	SpaceShip(float x_,float y_,float wid_,float hei_,float TIME_CHANGE){
		x=x_; y=y_; wid=wid_; hei=hei_;
		_src_y=y;
		dx=0;
		dy=0;
		src_mode=0;
		dest_mode=0;
		phi=ofRandom(TWO_PI);

		mwindow=(int)ofRandom(3,8);	
		for(int i=0;i<mwindow;++i){
			window_portion.push_back(ofRandom(.2,1));
		}
		mray=(int)ofRandom(4,12);
		// for(int i=0;i<mray;++i) ray_portion.append(ofRandom(.1,1));

		
		rot_vel=ofRandom(10,50);
		ray_wid=ofRandom(PI/50,PI/8);
		ship_far_pos=ofGetWidth()/3*ofRandom(1,2)*(ofRandom(2)<1?1:-1);
		
		start_frame=ofGetFrameNum();

		_anim_detect=FrameAnimation(ofRandom(60,80));
		_anim_detect.is_loop=true;
		_anim_detect.do_reverse=true;
		_anim_detect.Restart();
		
		_anim_catch=FrameAnimation(80);
		_anim_catch.is_loop=true;

		_anim_hide=FrameAnimation(80,TIME_CHANGE);
		//_anim_hide.is_loop=true;

		ray_bottom_y=ofRandom(3,5)*hei;

		_anim_fly=FrameAnimation(TIME_CHANGE);

		_calib_vel=ofVec2f();

		fly_dest=ofRandom(ofGetWidth());
	}
	void update(){
		_anim_detect.Update();
		_anim_catch.Update();
		_anim_fly.Update();
		_anim_hide.Update();

		if(_anim_fly.GetPortion()==1){
			src_mode=dest_mode;		
		} 


		float draw_portion=_anim_detect.GetPortion();



		dy=0;
		if(src_mode!=dest_mode){
			float t=_anim_fly.GetPortion();					
			y=_src_y+_mode_height*t;
			if(dest_mode==2) dx=(volc_dest.x-x)*t;

		}else{
			switch(dest_mode){
				case 0:
					x=x+(fly_dest-x)*draw_portion;
					ship_delta_x=ship_far_pos*draw_portion;
					dx=ship_delta_x;
					break;
				case 1:
					switch(_anim_catch._loop_count){
						case 0:
							dx=(_ray_dest_x-x)*_anim_catch.GetPortion();
							break;
						default:			
							dx=_ray_dest_x-x;			
							if(_anim_catch._loop_count>2) dx+=ship_far_pos*sin(_anim_catch.GetPortion()*HALF_PI+HALF_PI*(_anim_catch._loop_count-3));
							break;					
					}
					break;
				case 2:
					
					ship_delta_x=ship_far_pos*_anim_hide.GetPortion();
					dx=ship_delta_x;					
										
					break;
				case 3:
					//dx=ship_far_pos;
					break;
			}
		}

	}	
	void updateCollide(vector<ofVec2f>& pos_){

		_calib_vel.scale(0);
		
		for(ofVec2f p:pos_){
			//println(dist(p.x,p.y,x,y));
			if(ofDist(p.x,p.y,x,y)<wid){
				ofVec2f dir(x-p.x,y-p.y);
				dir.scale(1);
				_calib_vel+=dir;
			}
		}
		x+=_calib_vel.x;
		dy+=_calib_vel.y;
	}

	void changeMode(int mode_,float h_){
		switch(mode_){
			case 0:
				fly_dest=ofRandom(.2,.8)*ofGetWidth();
				_anim_detect.Restart();
			case 1:	
				_anim_catch.Restart();
				
				break;							
			case 2:
				ship_far_pos=ofGetWidth()*2;
				
				_anim_hide.Restart();
				break;
		}
		_mode_height=h_;
		_src_y=y;
		x+=dx;
		dx=0;

		_anim_fly.Restart();

		src_mode=dest_mode;
		dest_mode=mode_;

		//println("ship changeMode: "+dest_mode);

	}
	void goCatch(PAnimal& tar_){
		
		//ray_bottom_y=tar_.y+tar_.body_hei-y;
		
		_ray_dest_y=(tar_.y+tar_.body_hei)-y+tar_.delta_pos_y;
		_ray_dest_x=tar_.x+tar_.body_wid/2+tar_.delta_pos_x;
		
		_ray_animal_hei=tar_.body_hei;
		_ray_animal_wid=tar_.wid;

		fly_dest=tar_.fly_dest;
		_anim_fly=tar_._anim_mode_change;

		changeMode(1,0);

	}
	void draw(bool draw_fill){
		
		//update();
		
		//portion=((float)(ofGetFrameNum()-start_frame)/vel+phi)%TWO_PI;
		float draw_portion=_anim_detect.GetPortion();
		
		
		// int new_stage=(int)(portion/PI*2);
		// if(new_stage==0&&stage==3){
		// 	finished=true;
		// 	stage=new_stage;
		// }
		// stage=new_stage;
		//float pt=_anim_mode.GetPortion();

		


		ofPushStyle();
		if(draw_fill){
			ofSetColor(20,abs(sin(phi))*125.0*ofRandom(.8,1.2),phi/TWO_PI*20+180,255);
			////pg.noStroke();
		}else{
			// ofSetColor(255,255);
			ofNoFill();
			ofSetColor(255,200);
			// ofSetColorWeight(1);
		}

		ofPushMatrix();
		ofTranslate(x+dx,y+dy);

		//println(x+dx);

		ofPushMatrix();
		ofTranslate(0,0);
			ofBeginShape();

				ofVertex(0,0);
				ofBezierVertex(wid/8,-hei/8*draw_portion,
							wid/8,-hei/8*draw_portion,
							wid/4,-hei/12);
				ofBezierVertex(wid/4,-hei/3-hei/6*draw_portion,
							wid/4*3+wid/8,-hei/4-hei/6*draw_portion,
							wid/4*3,-hei/12);
				ofBezierVertex(wid/8*7,-hei/8*draw_portion,
							wid/8*7,-hei/8*draw_portion,
							wid,-hei/12);
				ofBezierVertex(wid/8*7,hei/5+hei/7*draw_portion,
							 wid/2*sin(phi),hei/4+hei/7*draw_portion,
							 0,0);

			ofEndShape();
			ofBeginShape();

				ofVertex(0,0);
				ofBezierVertex(wid/8,hei/8*draw_portion,
							wid/8,hei/8*draw_portion,
							wid/4,hei/12);
				ofBezierVertex(wid/4,hei/3+hei/16*draw_portion,
							wid/4*3+wid/8,hei/4+hei/6*draw_portion,
							wid/4*3,hei/12);
				ofBezierVertex(wid/12*7,hei/8*draw_portion,
							wid/12*11,hei/11*draw_portion,
							wid,0);

			ofEndShape();
		

		
		ofPopStyle();

		ofPushStyle();
		if(draw_fill){
			ofSetColor(255);
			//pg.noStroke();
		}else{
			ofNoFill();
			ofSetColor(0,200);
		}


		ofPushMatrix();
		ofTranslate(wid/3.5*draw_portion/5+wid/4,0);
		float wwindow=wid/2/(float)mwindow*.65;

		
		// window
		
		for(int i=0;i<mwindow;++i){
			ofBeginShape();
				float port=window_portion[i];
				ofVertex(wwindow*i,0);
				ofVertex(wwindow*(i+port),0);
				ofVertex(wwindow*(i+sin(port)),-hei/12);
				ofVertex(wwindow*i*cos(port),-hei/12);
			ofEndShape();
		}
		

		ofPopMatrix();
		ofPopStyle();

		ofPopMatrix();

		ofPushStyle();
		
			//light
			
			if(draw_fill){
				ofNoFill();
				ofSetColor(255,40);
			}else{
				ofNoFill();
				ofSetColor(255,200);
			}
			// float cur_phi=(draw_ray)?0:PI/8*sin((float)ofGetFrameNum()/25+phi);

			switch(src_mode){
				
				case 1:
					drawCatchRay(0,0,draw_fill);
					break;	
				case 0:			
					float cur_phi=getCurrentRayPhi();
					ofBeginShape();				
						for(int i=0;i<mray;++i){
							float rad=ofRandom(1)*ray_bottom_y*mwindow;
							float theta=ofRandom(0,ray_wid)+cur_phi;
							ofVertex(wid/2+rad*sin(theta),rad*cos(theta));
						}
					ofEndShape();
					break;
		
			}
		


		ofPopStyle();

		
		ofPopMatrix();

	}


	float getCurrentRayPhi(){

		float rot_portion=(float)ofGetFrameNum()/(rot_vel)+phi;
		rot_portion=fmod(rot_portion,TWO_PI);
		int rot_stage=(int)((rot_portion)/(PI/4));
		float ang=0;
		switch(rot_stage){
			case 0:
			case 1:
			case 2:
				rot_portion-=PI/4*rot_stage;
				ang=ray_wid*(sin((rot_portion)*4));
				break;
			case 3:
				rot_portion-=PI/4*3;
				ang=PI*sin((rot_portion)*2);
				break;
			case 4:
			case 5:
			case 6:
				rot_portion-=PI/4*rot_stage;
				ang=PI+ray_wid*(sin((rot_portion)*4));
				break;
			case 7:
				rot_portion-=PI/8*7;
				ang=PI+PI*sin((rot_portion)*2);
				break;
		}
		//println(ang/PI);
		return ang-ray_wid/2;

	}

	void drawCatchRay(float top_x,float top_y,bool draw_fill){
		
		ofPushStyle();
		if(draw_fill){
			ofSetColor(0,255,255);
			ofNoFill();
		}else{
			ofSetColor(255,255,0);
			ofNoFill();
		}

		ofPushMatrix();
		ofTranslate(top_x,0);
			float portion=_anim_catch.GetPortion();
			int stage=_anim_catch._loop_count;

			ray_cur_y=_ray_dest_y+hei;
			float tmp_wid=_ray_animal_wid;
			float close_wid=0;
			float animal_y=_ray_dest_y-_ray_animal_hei*1.5;
			float bottom_y=_ray_dest_y;

			switch(stage){
				case 0: // down
					ray_cur_y=(bottom_y-top_y)*portion+top_y;
					tmp_wid=ofLerp(0,wid,portion);
 					ofBeginShape();
						ofVertex(tmp_wid/2,ray_cur_y);
						if(ray_cur_y>animal_y) ofVertex(tmp_wid/2,animal_y);
						
						ofVertex(0,top_y);
						
						if(ray_cur_y>animal_y) ofVertex(-tmp_wid/2,animal_y);
						ofVertex(-tmp_wid/2,ray_cur_y);		
					ofEndShape();
					break;
 				case 1: // close
 					close_wid=tmp_wid/2*(1-portion);
 					ofBeginShape();
 						ofVertex(close_wid,bottom_y);
						ofVertex(tmp_wid/2,bottom_y);
						ofVertex(tmp_wid/2,animal_y);
						
						ofVertex(0,top_y);
						
						ofVertex(-tmp_wid/2,animal_y);
						ofVertex(-tmp_wid/2,bottom_y);		
						ofVertex(-close_wid,bottom_y);
					ofEndShape();
					break;
				case 2: // up
					ray_cur_y=(bottom_y-top_y)*(1-portion)+(top_y+_ray_animal_hei*1.5+hei*.5)*portion;
					ofBeginShape();
 						ofVertex(close_wid,ray_cur_y);
						ofVertex(tmp_wid/2,ray_cur_y);
						ofVertex(tmp_wid/2,ray_cur_y-_ray_animal_hei);
						
						ofVertex(0,top_y);
						
						ofVertex(-tmp_wid/2,ray_cur_y-_ray_animal_hei);
						ofVertex(-tmp_wid/2,ray_cur_y);		
						ofVertex(-close_wid,ray_cur_y);
					ofEndShape();
					break;
 				case 3: // stay up
 				default :
 					ray_cur_y=top_y+_ray_animal_hei*1.5+hei*.5;
					ofBeginShape();
 						ofVertex(close_wid,ray_cur_y);
						ofVertex(tmp_wid/2,ray_cur_y);
						ofVertex(tmp_wid/2,ray_cur_y-_ray_animal_hei);
						
						ofVertex(0,top_y);
						
						ofVertex(-tmp_wid/2,ray_cur_y-_ray_animal_hei);
						ofVertex(-tmp_wid/2,ray_cur_y);		
						ofVertex(-close_wid,ray_cur_y);
					ofEndShape();
					break;

			}
			
		ofPopMatrix();

		ofPopStyle();
	}

};



#endif