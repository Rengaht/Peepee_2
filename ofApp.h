#pragma once

#include "ofMain.h"
#include "StringSplit.h"
#include "FrameAnimation.h"
#include "PAnimal.h"
#include "DistortGraph.h"
#include "SpaceShip.h"
#include "Volcano.h"
#include "SpotLight.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		float _control_val[2];

		ofSoundPlayer  _sound_bgm[4];
		ofSoundPlayer  _sound_volc[5],_sound_et[5];
		FrameAnimation _bgm_fade_out,_bgm_fade_in;
		float _sound_vol;

		ofSerial _serial_ctrl;

		float TIME_CHANGE;
		
private:
		int M_CONTROL;
		int M_MODE;

		int mpas;
		vector<PAnimal> pas;
		vector<Volcano> volcs;
		vector<SpaceShip> ships;
		vector<SpotLight> spots;

		vector<int> ship_index;
		vector<int> volc_index;

		ofVec2f mid_pos;
		float dest_pos;

		vector<ofVec2f> _scene_pos;
		bool _start_dance;
		bool _show_spotlight;

		bool save_frame;

		//DistortGraphic fill_canvas,stroke_canvas;

		int dest_mode,last_mode;

		float angle_for_mode;

		FrameAnimation _anim_mode_change;
		FrameAnimation _anim_disco;

		ofVec2f min_moon_pos,max_moon_pos,moon_rad;

		ofFbo _fbo_fill,_fbo_stroke;
		DistortGraph _distort_fill;
		/*ofImage _dimage;
		ofPixels _dpixels;*/
		ofShader _dshader;
		
		void initPAnimal();
		void updateMidPos();
		vector<ofVec2f> tmp_poses;

		void changeMode();
		void changeMode(int mode);
		void changeSingleMode(int i);
		void changeSingleMode(int i,int mode_);
		
		void initControl();
		void initSound();
		void initSpotLight();
		void initVolcano();

		void triggerEvent(char ev);

		void calcMoonPos();
		void drawMoon(bool draw_fill,ofVec2f min_pos,ofVec2f max_pos);
		bool drawSpace();

		bool checkAllCurMode(int mode);
		bool checkAllDestMode(int mode);
};
