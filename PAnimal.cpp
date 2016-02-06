#include "PAnimal.h"

PAnimal::PAnimal(float x_,float y_,float wid_,float hei_,float TIME_CHANGE){
		
		x=x_; y=y_; wid=wid_; hei=hei_;
		src_y=y;
		mode_ang=0;

		float hhead_portion=ofRandom(.3,.5);
		head_wid=hhead_portion*wid;
		body_wid=wid-head_wid;
		
		body_hei=(hei)*ofRandom(.3,.5);
		float vhead_portion=ofRandom(.4,.8);
		head_hei=vhead_portion*body_hei;
		
		leg_hei=hei-body_hei;

		mleg=(int)ofRandom(3,6);
		leg_wid=body_wid/mleg*ofRandom(.2,.4);
		
		for(int i=0;i<mleg;++i){
			leg_span.push_back(ofRandom(0.05,0.5));
		}


		fcolor=ofColor(ofRandom(100,255),ofRandom(20,80)+100,ofRandom(20,85));
		
		phi=ofRandom(HALF_PI);
		
		
		_cur_mode=0;
		_dest_mode=0;
		elastic_stage=0;
		
		wheel_vel=27;
		skate_vel=60;
		run_vel=50;
		disco_vel=ofRandom(40,90);
		v_jump_vel=ofRandom(10,24);
		s_body_hei_incre=0;

		_src_ang=0;
		_dest_ang=0;

		disco_stage=-2;
		delta_pos_x=0;
		delta_pos_y=0;

		ship_dest=ofVec2f(0,ofRandom(y-hei));
		v_jump_dest_y=hei*ofRandom(.5,2);

		land_length=(int)ofRandom(20,50);//(int)(ofRandom(.5,1.2)*width);
		for(int i=0;i<land_length;++i){
				land_poses.push_back(ofRandom(-.2,.2));
		} 
		land_vel=ofRandom(4,6);
		land_index=0;
		_move_vel=ofVec2f(land_vel/10.0,0);


		elastic_strength=(float)land_vel/10*ofRandom(1,3);

		fly_dest=ofGetHeight()*ofRandom(2,2.5);
		jump_dest=ofGetHeight()*3-fly_dest;
		
	
		//_anim_mode_change=FrameAnimation(ofRandom(40,80),ofRandom(0,80));
		_anim_up=FrameAnimation(80,160);
		_anim_landing=FrameAnimation(ofRandom(30,90),TIME_CHANGE*.2);

		_anim_jump=FrameAnimation(ofRandom(12,30),TIME_CHANGE+ofRandom(10,20));
		_anim_jump.is_loop=true;
		_anim_jump.do_reverse=true;
		

		_anim_mode_change=FrameAnimation(TIME_CHANGE*ofRandom(.6,1.5),TIME_CHANGE*ofRandom(.2));
	
		_anim_float=FrameAnimation(ofRandom(250,300));
		_anim_float.is_loop=true;
		_anim_float.do_reverse=true;
		_anim_float.Restart();

		_anim_disco_leg=FrameAnimation(ofRandom(10,20));
		_anim_disco_leg.is_loop=true;
		
		_anim_disco_walk=FrameAnimation(TIME_CHANGE/2,TIME_CHANGE/2);
		
		disco_steps=(int)ofRandom(2,5);
		
	

		_calib_vel=ofVec2f(0,0);
		
		ship_dest=ofVec2f(0,0);
		volc_dest=ofVec2f(0,0);
		float_dest=ofVec2f(0,0);
		disco_dest=ofVec2f(0,0);

		_start_dance=false;

	}	

void PAnimal::draw(bool draw_fill){
		
		
		ofPushStyle();
		
		if(draw_fill){
		 	
		 	ofSetColor(fcolor);
		 	
		}else{
			ofNoFill();
			ofSetColor(ofColor::black);			
		}
		
		float draw_portion=1.5*abs(sin((float)ofGetFrameNum()/(120/land_vel)));


		float cur_body_wid=body_wid;
	

		
		int jump_stage=0;//sship.stage;//(int)((jump_portion)/(PI/2));
		float jump_pos_y=0;
		float jump_pos_x=0;
		
		s_body_hei_incre=0;

	
		

		ofPushMatrix();

		ofTranslate(x,y);
		ofTranslate(delta_pos_x,delta_pos_y);


		
		ofTranslate(body_wid/2,body_hei/2);
		ofRotate(mode_ang*RAD_TO_DEG);
		ofTranslate(-body_wid/2,-body_hei/2);

		
		

		

		ofBeginShape();
		ofVertex(0,0+s_body_hei_incre);
		
		ofBezierVertex(cur_body_wid/3,-body_hei*.3*draw_portion+s_body_hei_incre,
						 cur_body_wid/3*2,-body_hei*.3*draw_portion+s_body_hei_incre,
						 cur_body_wid,0);


		ofVec2f ear_base1=ofVec2f(head_hei,0);
		ear_base1.rotate(((-PI/3)*draw_portion-PI/6-PI/4)*RAD_TO_DEG);
		ofVec2f ear_base2=ofVec2f(head_hei,0);
		ear_base2.rotate((-PI/3*draw_portion-PI/4)*RAD_TO_DEG);
		
		ofBezierVertex(cur_body_wid+ear_base1.x,ear_base1.y,
					  cur_body_wid+ear_base2.x,ear_base2.y,
					  cur_body_wid,0);

		ofBezierVertex(cur_body_wid+head_wid/2,-body_hei*.1*draw_portion,
					 cur_body_wid+head_wid,head_hei+body_hei*.4*draw_portion,
					 cur_body_wid,head_hei);
	
		ofVertex(cur_body_wid*(1-(float)(0+leg_span[0])/mleg),body_hei);

		for(int i=0;i<mleg;++i){
			
			if(i>0) ofVertex(cur_body_wid*(1-(float)(i+leg_span[i])/mleg),body_hei);
			if(_cur_mode==0 || _cur_mode==1) continue;
			

			if(_cur_mode==2 && _dest_mode==2) drawJumpLeg(cur_body_wid*(1-(float)(i+leg_span[i])/mleg),body_hei,leg_wid,leg_hei,i);		
			else if(_cur_mode==3 && _dest_mode==3 && _start_dance) drawDiscoLeg(cur_body_wid*(1-(float)(i+leg_span[i])/mleg),body_hei,leg_wid,leg_hei,i);
			else if(!_start_dance) drawFloatLeg(cur_body_wid*(1-(float)(i+leg_span[i])/mleg),body_hei,leg_wid,leg_hei,i);

		}
		
		ofBezierVertex(-cur_body_wid*.1*draw_portion,body_hei,
					 -cur_body_wid*.1*draw_portion,0,
					 0,0+s_body_hei_incre);


		ofEndShape();
		if(!draw_fill){
			ofSetColor(fcolor.r/1.1,fcolor.g/1.1,fcolor.b/1.3);
			ofNoFill();
			ofBeginShape();
			for(int i=0;i<50;++i){
					// pg.strokeWeight(ofRandom(2));
					float n=ofNoise((i+ofGetFrameNum()+land_vel));
					float n2=ofNoise(i*land_vel);
					ofVertex(n*cur_body_wid,
							   ofMap(n2,0,1,-body_hei*.1,body_hei/2));
			
			}
			ofEndShape();
		}

		

		ofDrawBitmapString(ofToString(_cur_mode)+" "+ofToString(_dest_mode),0,0);

		ofPopMatrix();
		

		ofPopStyle();

		
		//update();
	}

void PAnimal::update(float ctrl_){

	
		_anim_up.Update();
		_anim_jump.Update();
		_anim_mode_change.Update();
		_anim_float.Update();
		_anim_disco_leg.Update();
		_anim_disco_walk.Update();
		_anim_landing.Update();
		
		if(_anim_mode_change.GetPortion()==1){
			_cur_mode=_dest_mode;	
		} 

		int jump_stage;
		float jump_portion=0;
		
		if(_cur_mode==0 || _dest_mode==0){	
			//mode_ang=mode_ang*(1-_anim_float.GetPortion());			 				
			mode_ang+=(ofRandom(50)<1)?(ofRandom(2)<1?-.05:.05):0;
			x+=ofRandom(2)<1?.5:-.5;
			y+=ofRandom(2)<1?.5:-.5;
		}

		if(_cur_mode!=_dest_mode){
			float t=_anim_mode_change.GetPortion();					
			
			// if(_dest_mode!=3) y=src_y+_mode_height*t;	
			// else y=src_y+_mode_height*constrain(t,0,.5)*2;
			y=src_y+_mode_height*t;	

			switch(_dest_mode){			 	
				case 2:
			 		delta_pos_x=(volc_dest.x-x)*t;
			 		break;
			 	case 3:
			 		//mode_ang=phi/2*_anim_disco_walk.GetPortion();
			 		mode_ang+=ofRandom(-.05,.05)*_anim_float.GetPortion();			 		

			 	 	y+=(disco_dest.y-y)*_anim_disco_walk.GetPortion();
					x+=(disco_dest.x-x)*_anim_disco_walk.GetPortion();

			 		break;
			 	case 0:
			 		mode_ang=mode_ang*(1-t);
			 	 	delta_pos_x=(float_dest.x-x)*t;
					delta_pos_y=(float_dest.y-y)*t;	
					break;
			}

		}else{
			switch(_cur_mode){				
				case 0:
					// float t2=_anim_float.GetPortion();
					
					// delta_pos_x=(float_dest.x-x)*t2;
					// delta_pos_y=(float_dest.y-y)*t2;	
					// if(abs(x-float_dest.x)>5) x=x+(float_dest.x-x)*_anim_float.GetPortion();
					// if(abs(y-float_dest.y)>5) y=y+(float_dest.y-y)*_anim_float.GetPortion();
					break;
				case 1:		
					mode_ang=0;	
					delta_pos_y=ship_dest.y*_anim_up.GetPortion();
					break;
				case 2:				
					
					if(_anim_landing.GetPortion()==1){
						jump_stage=_anim_jump._loop_count%4;
						jump_portion=_anim_jump.GetPortion();
						if(jump_stage==2||jump_stage==3){
							delta_pos_y=(volc_dest.y-y)-abs(v_jump_dest_y*jump_portion);	
						}else{
							delta_pos_y=(volc_dest.y-y)+abs(leg_hei*jump_portion);
						}
					}else{
						delta_pos_y=(volc_dest.y-y)*_anim_landing.GetPortion();					
					}
					break;
				case 3:
					
					
					if(!_start_dance){
						float t=_anim_float.GetPortion();
						x+=ofRandom(-2,2)*t;
						y+=ofRandom(2)*(1-t);
						mode_ang+=ofRandom(.1)*t;
						break;
					}else{
						
					}

					float disco_portion=_anim_disco_leg.GetPortion();
					int new_disco_stage=_anim_disco_leg._loop_count%4;
					
					//mode_ang=atan2(delta_pos_y,delta_pos_x);

					int disco_cycle=floor(_anim_disco_leg._loop_count/4);
					
					// if(disco_stage==-2) disco_stage=new_disco_stage;
					if(disco_stage==2 && new_disco_stage==3){
						// println(disco_cycle%(disco_steps*2)+"  "+x);
						int icycle=floor(fmod(disco_cycle,disco_steps*2));

						float dd=0;
						if(icycle>0 && icycle<disco_steps) dd+=leg_hei*sin(PI/6)*4+last_foot_base.y;//(first_foot_base.y);
						else if(icycle>disco_steps && icycle<disco_steps*2) dd-=leg_hei*sin(PI/6)*4+last_foot_base.y;

						x+=dd*cos(mode_ang);
						y+=dd*sin(mode_ang);

						// println(x);
						// println("go!");
					}
					disco_stage=new_disco_stage;

					disco_portion=fmod(disco_portion,4);
					float dt=0;
					if(fmod(disco_cycle,disco_steps*2)<disco_steps){
						if(disco_stage==0||disco_stage==3){
							dt-=last_foot_base.x;
						}else{
							dt-=((first_foot_base.x-first_foot_base.y)+last_foot_base.y);//-body_wid);//*(sin(disco_portion));
						}
					}else{
						if(disco_stage==1||disco_stage==2){
							dt-=last_foot_base.x;
						}else{
							dt-=((first_foot_base.x-first_foot_base.y)+last_foot_base.y);//-body_wid);//*(sin(disco_portion));
						}
					}	
					delta_pos_x=dt*cos(mode_ang);
					delta_pos_y=dt*sin(mode_ang);
					break;
			}
		}


		
}

void PAnimal::setStartDance(bool start_){
	if(!_start_dance && start_){
		x=disco_dest.x;
		y=disco_dest.y;
	}
	_start_dance=start_;
}

void PAnimal::checkSpeed(float ctrl_,ofVec2f mid_){
		
		_calib_vel.scale(0);

		ofVec2f c(mid_.x-x,mid_.y-y);
		ofVec2f dir(land_vel,0);
		if(_cur_mode==1) dir.rotate(PI/4*RAD_TO_DEG);
		else if(_cur_mode==2) dir.rotate(-PI/8*RAD_TO_DEG);

		float sp_=ofMap(ctrl_,0,1024,5,.5);
		//dir.setMag(constrain(c.mag(),0,1.5));
		dir.scale(sp_);

		float dot=c.dot(dir);
		
		if(dot>ofGetWidth()/4) _calib_vel+=dir;
		if(dot<-ofGetWidth()/4) _calib_vel-=dir;
		 

		// if(_cur_mode!=0) c.setMag(constrain(c.mag(),0,1));
		// else c.setMag(0);
		
		//_calib_vel=c.get();
		
	}
void PAnimal::checkCollide(float ctrl_,vector<ofVec2f> pos_){
		
		for(ofVec2f p:pos_){
			//println(dist(p.x,p.y,x,y));
			if(ofDist(p.x,p.y,x,y)<60){
				ofVec2f dir(x-p.x,y-p.y);
				dir.scale(ofMap(ctrl_,0,1024,0.5,5));
				_calib_vel+=dir;
			}
		}

	}

void PAnimal::changeMode(int mode_,float h_){
		switch(mode_){			
			case 1:
				_anim_up.Restart();		
				break;
			case 2:
				_anim_landing.Restart();
				_anim_jump.Restart();
				break;
			case 3:
				_anim_disco_leg.Restart();
				_anim_disco_walk.Restart();
				break;
			case 0:
				_anim_float.Restart();
				if(abs(TWO_PI-mode_ang)<mode_ang) mode_ang=-(TWO_PI-mode_ang);
				float_dest=ofVec2f(ofRandom(.2,.8)*ofGetWidth(),(.5+ofRandom(-.05,.5))*ofGetHeight());
				break;
		}
		
		_dest_mode=mode_;
		
		src_y=y+delta_pos_y;
		y=src_y;

		src_x=x+delta_pos_x;
		x=src_x;

		_mode_height=h_;
		last_jump_y=delta_pos_y;
		
		delta_pos_y=0;
		delta_pos_x=0;

		_anim_mode_change.Restart();

	}

	
void PAnimal::drawLegs(float kang,float fang,float lx,float ly,float lw,float lh,int leg_index){	
	ofVec2f knee_base(0,lh/2);
	knee_base.rotate(kang*RAD_TO_DEG);

	ofVec2f foot_base(0,lh/2);
	foot_base.rotate(fang*RAD_TO_DEG);
		

	ofVec2f foot_base2(-lw,lh/2);
	foot_base2.rotate(fang*RAD_TO_DEG);
		
		

	foot_base+=knee_base;
	foot_base2+=knee_base;
		
	ofBezierVertex(lx+knee_base.x,ly+knee_base.y,
					lx+foot_base.x,ly+foot_base.y,
					lx+foot_base.x,ly+foot_base.y);
	// vertex(lx+knee_base.x,ly+knee_base.y);
	// vertex(lx+foot_base.x,ly+foot_base.y);
		
	ofVertex(lx+foot_base2.x,ly+foot_base2.y);

	// vertex(lx-lw+knee_base.x,ly+knee_base.y);
	// vertex(lx-lw,ly);

	ofBezierVertex(lx-lw+knee_base.x,ly+knee_base.y,
					lx-lw+knee_base.x,ly+knee_base.y,
					lx-lw,ly);

	switch(_cur_mode){
		case 1:
			if(leg_index==0) first_foot_base=ofVec2f(lx-lw+foot_base.x,ly+foot_base.y);
			else if(leg_index==mleg-1) last_foot_base=ofVec2f(lx+lw+foot_base.x,ly+foot_base.y);
			break;
		case 2:
			if(leg_index==0) first_foot_base=ofVec2f(lx-lw+foot_base.x,ly+lh);
			else if(leg_index==mleg-1) last_foot_base=ofVec2f(lx+lw+foot_base.x,ly+lh);
			break;
		
	}
}


void PAnimal::drawJumpLeg(float lx,float ly,float lw,float lh,int leg_index){

	float frame_portion=_anim_jump.GetPortion();
	int stage=_anim_jump._loop_count%4;
		
	bool left=(leg_index>(mleg-1)/2);
	float aoffset=0;//((left)?-PI/6:PI/6);
		
	float kang=0;//-PI/2*(sin(frame_portion));
	float fang=0;//kang+PI/1.8;
		
	switch(stage){
		case 0:
			kang=-PI/3*(frame_portion)+aoffset;
			fang=kang+PI/1.7*(frame_portion);
			break;
		case 1:
			kang=-PI/3*(frame_portion)+aoffset;
			fang=kang+PI/1.7*(frame_portion);
			break;
		case 2:
			if(left) kang=0+PI/4*(frame_portion);
			else kang=0-PI/4*(frame_portion);
			fang=kang;//+PI/3*(1-abs(sin(frame_portion)));
			break;
		case 3:
			if(left) kang=0+PI/4*(frame_portion);
			else kang=0-PI/4*(frame_portion);//-PI/3*sin((frame_portion-PI/2*3)*2+PI/2*3);
			fang=kang;
			break;
				
	}

	drawLegs(kang,fang,lx,ly,lw,lh,leg_index);

}
void PAnimal::drawFloatLeg(float lx,float ly,float lw,float lh,int leg_index){
		
	bool left=(leg_index>(mleg-1)/2);
	float kang=(left)?-PI/4:PI/4;				
	drawLegs(kang,kang,lx,ly,lw,lh,leg_index);

}

void PAnimal::drawDiscoLeg(float lx,float ly,float lw,float lh,int leg_index){


	// float frame_portion=(float)frameCount/(disco_vel/land_vel/2)+phi;
	// int stage=(int)((frame_portion%TWO_PI)/(PI/2));
	// frame_portion%=TWO_PI;
	float frame_portion=_anim_disco_leg.GetPortion();
	//println(frame_portion);
	float stage=_anim_disco_leg._loop_count%4;
		
	float fang=-PI/6*sin(frame_portion*HALF_PI+stage*HALF_PI);
	bool isLeft=!(leg_index>(mleg-1)/2);// fang=-PI/8*(sin(frame_portion));//PI/6*(sin(frame_portion+PI/2));
	if(!isLeft) fang*=-1;
	float kang=fang;

	drawDancingLeg(kang,fang,lx,ly,lw,lh,leg_index);
			
}
void PAnimal::drawDancingLeg(float kang,float fang,float lx,float ly,float lw,float lh,int leg_index){	
	ofVec2f knee_base(0,lh/2);
	knee_base.rotate(kang*RAD_TO_DEG);

	ofVec2f foot_base(0,lh/2);
	foot_base.rotate(fang*RAD_TO_DEG);
		

	ofVec2f foot_base2(-lw,lh/2);
	foot_base2.rotate(fang*RAD_TO_DEG);
		
	foot_base+=knee_base;
	foot_base2+=knee_base;
		
	float frame_portion=(float)ofGetFrameNum()/(disco_vel/land_vel/2)+phi;
	float draw_portion=sin(frame_portion);//*random(-1,1);

	ofBezierVertex(lx+knee_base.x+lw*draw_portion,ly+knee_base.y,
					lx+foot_base.x+lw*draw_portion,ly+foot_base.y,
					lx+foot_base.x,ly+foot_base.y);
	
	ofVertex(lx+foot_base2.x,ly+foot_base2.y);

	ofBezierVertex(lx-lw+knee_base.x+lw*draw_portion,ly+knee_base.y,
					lx-lw+knee_base.x+lw*draw_portion,ly+knee_base.y,
					lx-lw,ly);

	
	if(leg_index==0) first_foot_base=ofVec2f(lx-lw+foot_base.x,lx-lw+lh*sin(PI/6));
	else if(leg_index==mleg-1) last_foot_base=ofVec2f(lx+lw+foot_base.x,lx+lw-lh*sin(PI/6));
				
		
}

bool PAnimal::operator < (const PAnimal& pa) const{
	return y<pa.y;
}
