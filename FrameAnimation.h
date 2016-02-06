#ifndef FRAME_ANIMATION
#define FRAME_ANIMATION


#include <math.h>  
#include "ofMath.h"

class FrameAnimation{
	public:
		
		//static float millis_per_frame;
		int _loop_count;
		float ani_t;
		bool is_loop;
		bool do_reverse;
		
		FrameAnimation(){
			setup(1,0,true);
		}
		FrameAnimation(float set_length){
			setup(set_length,0.0f,false);
		}
		/*FrameAnimation(float set_length,bool elastic){
			setup(set_length,0.0f,elastic);
		}*/
		FrameAnimation(float set_length,float set_delay){
			setup(set_length,set_delay,true);
		}
		FrameAnimation(float set_length,float set_delay,float set_start,float set_end){
			setup(set_length,set_delay,true);
			setPos(set_start,set_end);
		}

		void Start(){
			ani_start=true;
			//ani_t=0;
		}
		
		bool isFinished(){
			return (ani_t==1);
		}
		bool isPlaying(){
			return ani_start && ani_t<1;
		}
		float GetPortion(){
			if(!ani_start) return 0;
			if(ani_t<0) return 0;
			if(ani_t==1) return 1;
			//return ofClamp(sinf(ani_t*(HALF_PI)),0,1);
			float r=0;

			if(is_elastic){
				float k=0.5f;			
				r=(0.5f-0.51f*cosf(PI*ani_t+k*ani_t-k*0.5f));								
			}else{
				float t=ani_t*2.0;
				if(t<1) return 1.0/2.0*pow(t,4)+0.0;
				t-=2;
				r=-1.0/2.0*(pow(t,4)-2)+0.0;
			}

			if(do_reverse) return (_loop_count%2==1)?(1-r):r;
			else return r;
		}
		float GetPos(){
			if(ani_t<0) 
				return start_pos;
			return GetPortion()/ani_vel;
		}
		void Update(){
			

			if(!ani_start) return;
			
			if(ani_t<1-ani_vel) ani_t+=(ani_vel);
			else{
				ani_t=1;
				if(is_loop){					
					_loop_count++;
					Reset();
					Start();
				}
			}
		}
		void Reset(){
			ani_start=false;
			if(!is_loop)ani_t=-delay_t*ani_vel;
			else ani_t=0;
			// Start();
		}
		void Restart(){
			ani_start=false;
			ani_t=-delay_t*ani_vel;
		
			Start();
			_loop_count=0;
		}
		
		void setPos(float set_start,float set_end){
			start_pos=set_start;
			end_pos=set_end;
		}
		void setEndPos(float set_end){
			start_pos=end_pos;
			end_pos=set_end;
		}
		void setDelay(float set_delay){
			delay_t=set_delay;
		}
		float getEndPos(){
			return end_pos;
		}
		void setElastic(bool set_elastic){
			is_elastic=set_elastic;
		}
		void setLoop(bool set_loop){
			is_loop=set_loop;
		}
		

	private:
		float ani_vel;
		float delay_t;

		float start_pos,end_pos;

		bool ani_start;
		bool is_elastic;
		

		

		void setup(float set_length,float set_delay,bool set_elastic){

			ani_vel=1.0/set_length;
			//ofLog()<<ani_vel;

			delay_t=set_delay;
			
			is_elastic=set_elastic;
			is_loop=false;
			do_reverse=false;
			_loop_count=0;

			Reset();
		}
};


#endif