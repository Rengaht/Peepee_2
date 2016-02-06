#ifndef PANIMAL_H
#define PANIMAL_H

#include "ofMain.h"
#include "FrameAnimation.h"

class PAnimal{

private:
	float head_wid,head_hei;
	
	
	int mleg;
	vector<float> leg_span;

	float tail_wid,tail_hei;
	ofColor fcolor;
	
	float phi;
	float elastic_strength;
	int elastic_stage;

	vector<float> land_poses;
	int land_length;
	float land_vel;
	int land_index;

	

	ofVec2f _move_vel,_calib_vel;

	
	
	float wheel_vel;
	float skate_vel;
	float run_vel;

	
	float v_jump_vel;

	float s_body_hei_incre;

	ofVec2f first_foot_base;
	ofVec2f last_foot_base;

	FrameAnimation _anim_float;
	FrameAnimation _anim_landing;
	
	FrameAnimation _anim_disco_leg;
	FrameAnimation _anim_disco_walk;
	FrameAnimation _anim_up;

	float _src_ang,_dest_ang;

	float disco_steps;
	float src_y,src_x;
	
	float last_jump_y;
	float disco_vel;
	int disco_stage;

	float mode_ang,_mode_height;

	bool _start_dance;

	void drawLegs(float kang,float fang,float lx,float ly,float lw,float lh,int leg_index);
	void drawJumpLeg(float lx,float ly,float lw,float lh,int leg_index);
	void drawFloatLeg(float lx,float ly,float lw,float lh,int leg_index);
	void drawDiscoLeg(float lx,float ly,float lw,float lh,int leg_index);
	void drawDancingLeg(float kang,float fang,float lx,float ly,float lw,float lh,int leg_index);

	

public:
	float x,y,wid,hei;
	float body_wid,body_hei;
	
	float leg_wid,leg_hei;

	float delta_pos_x,delta_pos_y;
	float fly_dest;

	ofVec2f ship_dest,volc_dest,disco_dest,float_dest;

	int _cur_mode,_dest_mode;
	
	
	FrameAnimation _anim_mode_change;
	
	float v_jump_dest_y;
	float jump_dest;
	FrameAnimation _anim_jump;


	PAnimal(float x_,float y_,float wid_,float hei_,float TIME_CHANGE);

	void draw(bool draw_fill);
	void update(float ctrl_);

	void checkSpeed(float ctrl_,ofVec2f mid_);
	void checkCollide(float ctrl_,vector<ofVec2f> pos_);

	void changeMode(int mode_,float h_);

	bool operator < (const PAnimal& pa) const;


	void setStartDance(bool start_);
};



#endif