#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofHideCursor();

	_distort_fill=DistortGraph(20);
	_distort_fill.transform_mag=2.2;

	ofDisableArbTex();

	_fbo_fill.allocate(ofGetWidth()*_distort_fill.getWeight(),ofGetHeight()*_distort_fill.getWeight(),GL_RGBA);
	_fbo_stroke.allocate(ofGetWidth(),ofGetHeight(),GL_RGBA);

	_fbo_fill.begin();
	ofClear(255,255,255,0);
	_fbo_fill.end();

	_fbo_stroke.begin();
	ofClear(255,255,255,0);
	_fbo_stroke.end();

	
//#ifdef TARGET_OPENGLES
//	_dshader.load("shaders_gles/dshader.vert","shaders_gles/dshader.frag");
//#else
//	_dshader.load("shaders/dshader.vert","shaders/dshader.frag");
//#endif

	M_CONTROL=1;
	M_MODE=4;
	
	mpas=5;
	last_mode=0;
	dest_mode=-1;
	angle_for_mode=0;

	TIME_CHANGE=80;
	_anim_mode_change=FrameAnimation(TIME_CHANGE*.5);
	_anim_disco=FrameAnimation(30,TIME_CHANGE);
	_anim_disco.is_loop=true;

	float width=ofGetWidth();
	float height=ofGetHeight();
	_scene_pos.push_back(ofVec2f(width/2,height/2));
	_scene_pos.push_back(ofVec2f(width/2,height/2));
	_scene_pos.push_back(ofVec2f(width/2,-height*2+height/2));
	_scene_pos.push_back(ofVec2f(width/2,-height*4+height/2));
	

	initControl();
	initSound();
	initPAnimal();

	changeMode(0);
	
	ofEnableSmoothing();
	//ofDisableAntiAliasing();

	save_frame=false;
	
}
void ofApp::initPAnimal(){
	
	pas.clear();
	ships.clear();
	ship_index.clear();
	volc_index.clear();

	float tmp_pos=0;
	float tmpv=0;
	for(int i=0;i<mpas;++i){
		float tmp_h=ofRandom(0.6,1.2)*64;
		
		pas.push_back(PAnimal(ofRandom(.1,.9)*ofGetWidth(),ofRandom(.66,1)*ofGetHeight(),96.0*ofRandom(.6,1.4),tmp_h,TIME_CHANGE));
		
		float vw=ofGetWidth()/5*ofRandom(.6,1.4);
		tmpv+=vw*ofRandom(.5,1);
		ships.push_back(SpaceShip(ofRandom(ofGetWidth()),ofRandom(ofGetHeight()/3),96.0*ofRandom(.6,1.4),ofRandom(0.6,1.2)*64,TIME_CHANGE));
		
		ship_index.push_back(i);
		volc_index.push_back(i);
		
	}
	mpas=pas.size();

}
void ofApp::initSpotLight(){
	spots.clear();

	for(int i=0;i<4;++i){
		ofVec2f p(ofRandom(1,1.2)*ofDist(ofGetWidth()/2,ofGetHeight()/2,0,0));
		p.rotate(ofRandom(TWO_PI)*RAD_TO_DEG);

		spots.push_back(SpotLight(ofGetWidth()/2+p.x,_scene_pos[3].y+p.y,
								3*ofDist(p.x,p.y,0,0),ofRandom(PI/4,PI/2.5),
								atan2(-p.y,-p.x),TWO_PI+atan2(-p.y,-p.x),
								30,TIME_CHANGE+30*i,TIME_CHANGE+30*4));		
	}

}
void ofApp::initVolcano(){

	volcs.clear();
	

	float tmpv=50;
	for(int i=0;i<mpas;++i){
		PAnimal pa=pas[i];
		float vw=ofGetWidth()/8*ofRandom(.6,1.4);
		//tmpv=constrain(tmpv,vw/2,width);
		//tmpv++random(vw)
		volcs.push_back(Volcano(tmpv,_scene_pos[2].y+ofRandom(-.1,.3)*ofGetHeight(),vw,ofRandom(5,20),
								pa._anim_jump,pa.jump_dest,TIME_CHANGE));
		
		tmpv+=ofRandom(.5,1)*vw;			
	}
	calcMoonPos();
}

		
		
void ofApp::initControl(){
	
	_control_val[0]=0;
	_control_val[1]=512;	
	

	_serial_ctrl.listDevices();

#ifdef TARGET_WIN32
	_serial_ctrl.setup(1,9600);
#endif
#if defined( TARGET_LINUX )
		_serial_ctrl.setup("/dev/ttyACM0",9600);
#endif

}

void ofApp::initSound(){

	_sound_bgm[0].loadSound("sound/PIPI_2_ET_background.wav");
	_sound_bgm[2].loadSound("sound/PIPI_2_volcano_background.wav");
	_sound_bgm[3].loadSound("sound/PIPI_disco.wav");
	for(int i=0;i<4;++i) _sound_bgm[i].setLoop(true);

	for(int i=0;i<5;++i) _sound_et[i].loadSound("sound/PIPI_2_ET_catch_"+ofToString(i+1)+".wav");
	for(int i=0;i<5;++i) _sound_volc[i].loadSound("sound/PIPI_2_volcano_shoot_"+ofToString(i+1)+".wav");


	_sound_vol=1.0;
	_bgm_fade_in=FrameAnimation(30,30);
	_bgm_fade_out=FrameAnimation(30);

}

//--------------------------------------------------------------
void ofApp::update(){
	 ofEnableAlphaBlending();
	
	for(auto& pa:pas) pa.update(_control_val[3]);
	updateMidPos();
	vector<ofVec2f> poses;
	for(auto& ship:ships){
		ship.update();	
		poses.push_back(ofVec2f(ship.x+ship.dx,ship.y));
	}
	//update ship collide	
	for(auto& ship:ships) ship.updateCollide(poses);

	for(auto& volc:volcs) volc.update();
	
	_anim_mode_change.Update();
	//if(_anim_mode_change.GetPortion()==1) last_mode=dest_mode;
	_distort_fill.update();

	if(dest_mode==3){
		_anim_disco.Update();
		if(_anim_disco._loop_count>3){
			_start_dance=true;
			for(auto& pa:pas){
				pa.setStartDance(true);				
			}
		}
	}else if(dest_mode==0 || dest_mode==1){

		for(int i=0;i<mpas;++i){
			int pi=ship_index[i];
			if(pas[i]._cur_mode==1 && ships[pi]._anim_catch._loop_count>2){
				pas[i].delta_pos_x=ships[pi].dx+ships[pi].x-pas[i].x-pas[i].body_wid/2;
				pas[i].delta_pos_y+=ships[pi].dy;
			}
		}
	}
	for(auto& sp:spots) sp.update();

	int nmode=(dest_mode+1)%M_MODE;
	if(dest_mode>-1 && checkAllCurMode(nmode) && _anim_mode_change.GetPortion()==1) changeMode();
	

	//-------- update sound -------//
	_bgm_fade_in.Update();
	_bgm_fade_out.Update();
	if(last_mode>-1){
		_sound_bgm[last_mode].setVolume(_sound_vol*(1-_bgm_fade_out.GetPortion()));
		if(_bgm_fade_out.GetPortion()==1) _sound_bgm[last_mode].stop();
	}
		_sound_bgm[dest_mode].setVolume(_sound_vol*_bgm_fade_in.GetPortion());
	


	//----- handel serail control -----//
	if(_serial_ctrl.isInitialized() && _serial_ctrl.available()){
		/*unsigned char bytesReturned[2];
		unsigned char bytesReadString[24];
		int nRead=0;
		int nTimesRead=0;
		int nBytesRead=0;

		memset(bytesReadString, 0, 24);
		memset(bytesReturned, 0, 2);
		
		while((nRead=_serial_ctrl.readBytes(bytesReturned,2))>0){
			unsigned char *endchar=(unsigned char*)memchr(bytesReturned,'#',3);
			if(endchar!=NULL){
				memcpy(bytesReadString+nBytesRead,bytesReturned,endchar-bytesReturned);
				break;
			}
			memcpy(bytesReadString+nBytesRead,bytesReturned,nRead);
			nBytesRead+=nRead;		
		};	
		cout<<ofToString(bytesReadString)<<endl;*/
		
		string str=readSerialString(_serial_ctrl, '|');

		if(str.length()>0){
			string read_string=str;//ofToString(bytesReadString);
			vector<string> val=split(read_string,'|');
			cout<<ofToString(val)<<endl;
			if(val.size()==M_CONTROL){
				for(int i=0;i<val.size();++i){
					//int n=ofToInt(val[i]);
					triggerEvent(ofToChar(val[i]));
					
				}
				//cout<<ofToString(_control_val)<<endl;
			}
		}
	}
}
void ofApp::triggerEvent(char ev){
	switch(ev){
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			changeSingleMode(ev-'1');
			break;
		case '7':
			_sound_vol-=.2;
			_sound_vol=ofClamp(_sound_vol,0,2);
			_sound_bgm[dest_mode].setVolume(_sound_vol);
			break;
		case '9':
			_sound_vol+=.2;
			_sound_vol=ofClamp(_sound_vol,0,2);
			_sound_bgm[dest_mode].setVolume(_sound_vol);
			break;
		case '*':
			initPAnimal();
			changeMode(0);
			break;
	}
	


}

void ofApp::updateMidPos(){
	if(last_mode>-1)
		mid_pos=ofVec2f(ofLerp(_scene_pos[last_mode].x,_scene_pos[dest_mode].x,_anim_mode_change.GetPortion()),
						ofLerp(_scene_pos[last_mode].y,_scene_pos[dest_mode].y,_anim_mode_change.GetPortion()));
	else mid_pos=_scene_pos[dest_mode];
}
void ofApp::changeMode(){
	changeMode((dest_mode+1)%M_MODE);
}

void ofApp::changeMode(int mode){

	if(_anim_mode_change.isPlaying()) return;
	cout<<"change mode: "<<mode<<endl;

	/*_sound_bgm[last_mode].stop();
	if(_sound_bgm[mode].isLoaded()) _sound_bgm[mode].play();*/

	switch(mode){
		case 0:
			_start_dance=false;
			initPAnimal();

			
			initSpotLight();
			initVolcano();

			random_shuffle(ship_index.begin(),ship_index.end());
			random_shuffle(volc_index.begin(),volc_index.end());
			break;
		case 3:
			_anim_disco.Restart();
			_start_dance=false;
			for(auto& pa:pas) pa.setStartDance(false);

			for(auto& sp:spots) sp.restart();
			for(auto& v:volcs) v.reset();
			_show_spotlight=true;
			break;
	}
	last_mode=dest_mode;
	dest_mode=mode;		
	_anim_mode_change.Restart();


	if(mode!=1 && _sound_bgm[mode].isLoaded()){
		if(last_mode>-1){
			_bgm_fade_out.Restart();
		}
		_bgm_fade_in.Restart();
		
		
		_sound_bgm[mode].play();
		_sound_bgm[mode].setVolume(_sound_vol);
		if(mode!=3) _sound_bgm[mode].setPosition(ofRandom(1));
		
	}
	if(mode==2) last_mode=0;

}

void ofApp::changeSingleMode(int i){

	int nmode=(dest_mode+1)%M_MODE;
	if(nmode==0 || nmode==2){

		if(nmode==0){
			if(!_start_dance) return;
			else{
				_show_spotlight=false;
				changeMode();
				//_sound_bgm[3].stop();
			}
		}

		if(checkAllCurMode(dest_mode))
			for(int x=0;x<mpas;++x) changeSingleMode(x,nmode);

	}else if(nmode==1){
		if(pas[i]._dest_mode!=nmode) changeSingleMode(i,nmode);	

	}else if(nmode==3){
		
		
		int k=0;
		while(!checkAllDestMode(nmode) && k<i+1){
			int ri=floor(ofRandom(mpas));
			if(pas[ri]._dest_mode<nmode){
				changeSingleMode(ri,nmode);	
				k++;	
			}else{
				cout<<ri<<" "<<pas[ri]._dest_mode<<endl;
			}

		}
	}			

}
void ofApp::changeSingleMode(int i,int mode_){
	

	if(mode_==pas[i]._dest_mode) return;

	int vi=volc_index[i];
	int pi=ship_index[i];

	/*PAnimal pa=pas[i];
	SpaceShip s=ships[pi];
	Volcano v=volcs[vi];*/
			
	
	if(mode_==1){

		_sound_et[pi].play();
		_sound_et[pi].setVolume(_sound_vol);

		ships[pi].goCatch(pas[i]);
		pas[i].ship_dest.y=(ships[pi].y+ships[pi].hei*.5+pas[i].body_hei*.5)-(pas[i].y+pas[i].delta_pos_y);
	}
	else if(mode_==2){
		
		volcs[vi].setPP(pas[i]);
		

		ships[pi].volc_dest=ofVec2f(volcs[vi].x,0);
		pas[i].volc_dest=ofVec2f(volcs[vi].x-pas[i].body_wid/2,volcs[vi].y-pas[i].hei-pas[i].leg_hei*.5);

	}else if(mode_==3){

		_sound_volc[vi].play();		
		_sound_volc[vi].setVolume(_sound_vol);

		pas[i].disco_dest=ofVec2f(_scene_pos[3].x+ofRandom(-.35,.35)*ofGetWidth(),_scene_pos[3].y+ofRandom(-.25,.25)*ofGetHeight());
	}else{
		float total_=_scene_pos[M_MODE-1].y-_scene_pos[0].y;
	 	pas[i].y-=total_;
	 	pas[i].changeMode(0,0);
	 	
	 	ships[pi].y-=total_;	 	
	 	ships[pi].changeMode(0,0);
	 	
	 	return;
	}

	pas[i].changeMode(mode_,_scene_pos[mode_].y-_scene_pos[pas[i]._cur_mode].y);
	ships[pi].changeMode(mode_,_scene_pos[mode_].y-_scene_pos[pas[i]._cur_mode].y);
	volcs[vi].changeMode(mode_,_scene_pos[mode_].y-_scene_pos[pas[i]._cur_mode].y);

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofBackground(0);


	bool to_break=false;
	if(dest_mode==3 &&_anim_mode_change.GetPortion()==1){		
		to_break=drawSpace();		
		if(to_break){
			for(auto& pa:pas) pa.y=_scene_pos[3].y+ofRandom(-.5,.1)*ofGetHeight();
		}
	}
	_fbo_fill.begin();
		if(dest_mode==3 &&_anim_mode_change.GetPortion()==1){
			ofPushStyle();
			ofSetColor(0,0,0,20);
			ofRect(0,0,_fbo_fill.getWidth(),_fbo_fill.getHeight());
			ofPopStyle();
		}else ofClear(255,255,255,0);
		
		ofPushMatrix();
		ofTranslate(ofGetWidth()/2-mid_pos.x,ofGetHeight()/2-mid_pos.y);
		
		
		if(dest_mode==2 || (last_mode==2 && !_start_dance)) drawMoon(true,min_moon_pos,max_moon_pos);
		
		if(!to_break){	 
			for(auto& pa:pas) pa.draw(true);
			for(auto& ship:ships) ship.draw(true);

		}
		
		int num=floor(mid_pos.y/100);
		for(int i=0;i<5;++i){
			int k=num-(i-2);
			ofSetColor(255,220);
			ofDrawBitmapString(ofToString(ofGetHeight()-k*100,4),10,k*100);
		}

		ofPopMatrix();
	_fbo_fill.end();

	_fbo_stroke.begin();
		ofClear(255,255,255,0);
		ofPushMatrix();
		ofTranslate(ofGetWidth()/2-mid_pos.x,ofGetHeight()/2-mid_pos.y);
		
		if(dest_mode==2|| dest_mode==2) drawMoon(false,min_moon_pos,max_moon_pos);
	
		if(!to_break){	 
			for(auto& pa:pas) pa.draw(false);
			for(auto& ship:ships) ship.draw(false);			

			if(dest_mode==3 && _show_spotlight) 
				for(auto& sp:spots) sp.draw();
		}	
			
		ofPopMatrix();
	_fbo_stroke.end();

	
	ofPushMatrix();
		_fbo_fill.getTextureReference().bind();
		_distort_fill._mesh.draw();
		_fbo_fill.getTextureReference().unbind();
		//_distort_fill._mesh.drawWireframe();
	ofPopMatrix();

	_fbo_stroke.draw(0,0);
	
	if(save_frame) ofSaveFrame();

	ofPushStyle();
	ofSetColor(ofColor::red);		
		ofDrawBitmapString(ofToString(ofGetFrameRate()),20,20);
		ofDrawBitmapString(ofToString(last_mode)+" -> "+ofToString(dest_mode),80,20);		
		for(int i=0;i<M_CONTROL;++i) ofDrawBitmapString(ofToString(_control_val[i]),20,30+10*i);
		ofDrawBitmapString(ofToString(_sound_vol),20,40);
	ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case 's':
			save_frame=!save_frame;
			break;
		/*case 'd':
			changeMode();
			break;*/
		
		case 'z':
			//_distort_fill.reset();
			initPAnimal();
			changeMode(dest_mode);
			
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':		
			int i=key-'1';
			changeSingleMode(i);
			break;
	}
}
#pragma region OF_CONTROL
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
#pragma endregion

void ofApp::drawMoon(bool draw_fill,ofVec2f min_pos,ofVec2f max_pos){
	float draw_portion=abs(sin((float)ofGetFrameNum()/50));
	ofPushStyle();
	if(draw_fill){
		ofColor fc(200+50*draw_portion,40*draw_portion+120,80);
		ofSetColor(fc);
	}else{
		ofSetColor(0);
		ofNoFill();
		//pg.fill(255,0,0);
	}

	ofVec2f cent(ofLerp(min_pos.x,max_pos.x,.5),ofLerp(min_pos.y,max_pos.y,.5));
	float ang=0;//atan2(max_pos.y-min_pos.y,max_pos.x-min_pos.x);

	ofPushMatrix();

	float rx=min_pos.distance(max_pos);
	float ry=(max_pos.y-min_pos.y);

	

	ofPushMatrix();
	//pg.line(min_pos.x,min_pos.y,max_pos.x,max_pos.y);
	
	ofTranslate(min_pos.x,cent.y);
	// pg.rotate(ang);
	// pg.line(rx,0,0,0);
	// pg.rect(-rx,-ry,rx*2,ry*2);

	//pg.translate(x_,0);
	
	ofBeginShape();
		ofVertex(0,ry*(1+.2*ofNoise(cent.y)));
		ofBezierVertex(rx*.55,ry,
						rx,ry,
						rx,0);
		if(draw_fill){
			ofBezierVertex(rx,-ry*.55,
							rx,-ry,
							0,-ry*ofNoise(cent.x));		
			ofVertex(0,ry*ofNoise(cent.x));
		}else{
			ofBezierVertex(rx,ry,
							rx*ofNoise(rx),ry*.86,
							rx*.05,ry*ofNoise(cent.x));
		}
			
	ofEndShape();
	ofPopMatrix();

	for(auto& vol:volcs){
		vol.draw(draw_fill,true,false);	
	}

	ofPopMatrix();


	ofPopStyle();


}

bool ofApp::drawSpace(){
	if(!_start_dance && _anim_disco.ani_t<.5 && _anim_disco.ani_t>0){
		//if(_anim_disco._loop_count>0){
			for(PAnimal pa:pas){
				pa.x=ofRandom(ofGetWidth());
				pa.y=_scene_pos[3].y+ofRandom(-.5,.5)*ofGetHeight();
			}
		//}
		return true;	
	} 
	if(!_start_dance) return false;


	int noise_scale=20;
	// pg.beginDraw();
	// pg.background(0);
	
	ofVec3f color_separate(0.33,0.33,0.33);
	int cycle=(!_start_dance)?_anim_disco._loop_count%3:(int)((float)ofGetFrameNum()/80)%3;
	if(cycle==0) color_separate.x=1;
	if(cycle==1) color_separate.y=1;
	if(cycle==2) color_separate.z=1;

	ofPushStyle();
	float width=ofGetWidth();
	float height=ofGetHeight();
	int fr=ofGetFrameNum();
	for(int x=0;x<=width;x+=noise_scale)
		for(int y=0;y<=height;y+=noise_scale){

			if(ofRandom(2)<1) continue;
			//if(noise(x)>.66) continue;
			float val=0;
    		if(!_start_dance) val=ofNoise(x,y);
    		else val=ofNoise(x+fr,y+fr);

    		ofSetColor(val*220*color_separate.x,val*220*color_separate.y,val*220*color_separate.z,120);
			//ofNoFill();
			ofSetRectMode(OF_RECTMODE_CENTER);
			float w=noise_scale*pow(val,2);
    		ofRect(x,y,w,w);
  		}

	ofPopStyle();

  	return false;

}

void ofApp::calcMoonPos(){
	float minx=ofGetWidth();	
	float maxx=0;
	float miny=ofGetHeight();
	float maxy=_scene_pos[2].y;
	for(Volcano v:volcs){
		if(v.x-v.wid/2<minx) minx=v.x-v.wid/2;
		if(v.x+v.wid/2>maxx) maxx=v.x+v.wid/2;

		if(v.y<miny) miny=v.y;
		if(v.y+v.hei>maxy) maxy=v.y+v.hei;
	}
	//moon_pos=new PVector((minx+maxx)/2,maxy);
	//min_moon_pos=new PVector(minx,miny);
	min_moon_pos=ofVec2f(min(0.0f,minx),miny);
	max_moon_pos=ofVec2f(maxx,maxy);

	// println(min_moon_pos);
	// println(max_moon_pos);

	moon_rad=ofVec2f((maxx-minx)/2,maxy-miny);


}

bool ofApp::checkAllCurMode(int mode){
	for(auto& pa:pas){
		if(pa._cur_mode!=mode) return false;
	}
	return true;
}
bool ofApp::checkAllDestMode(int mode){
	for(auto& pa:pas){
		if(pa._dest_mode!=mode) return false;
	}
	return true;
}