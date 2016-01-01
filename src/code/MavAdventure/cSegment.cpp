
#include "cMainManager.h"
#include "cGameRoom.h"
#include "cSegment.h"
#include "util.h"

#include <QDebug>



int gNumSegs;

cSegment::cSegment()
{
	m_pLine = NULL;
	m_S = QVector2D(0,0);
	m_E = QVector2D(0,0);
	m_T = QVector2D(0,0);
	m_N = QVector2D(0,0);
	m_ToRemove = false;
	m_Dist = 0.f;
	
	
	
	m_Idx = gNumSegs;
	gNumSegs++;
	
	m_Visible = true;
}

cSegment::cSegment(const QVector2D &s, const QVector2D &e)
{
	m_pLine = NULL;
	m_S = s;
	m_E = e;
	m_T = e-s;
	m_N.setX( m_T.y());
	m_N.setY(-m_T.x());
	m_N.normalize();
	m_ToRemove = false;
	
	m_Dist = QVector2D::dotProduct(m_S,m_N);
	
	m_Visible = true;
}

cSegment::~cSegment()
{
	
}

void cSegment::Step()
{
	int R,L,HMarg, U,D,VMarg;
	
	if (!m_pMyRoom->m_Block[EBS_LEFT] && !m_pMyRoom->m_Block[EBS_RIGHT])
	{
		HMarg = m_pMyRoom->m_Size[XX]-640;
		HMarg /= 2;
		R = 640+HMarg;
		L = -HMarg;
		if (std::min(m_E[XX],m_S[XX]) > R)
		{
			m_S[XX] -= m_pMyRoom->m_Size[XX];
			m_E[XX] -= m_pMyRoom->m_Size[XX];
			CalcMe();
			
		}
		if (std::max(m_E[XX],m_S[XX]) < L)
		{
			m_S[XX] += m_pMyRoom->m_Size[XX];
			m_E[XX] += m_pMyRoom->m_Size[XX];
			CalcMe();
		}
	}
	if (!m_pMyRoom->m_Block[EBS_DOWN] && !m_pMyRoom->m_Block[EBS_UP])
	{
		VMarg = m_pMyRoom->m_Size[YY]-480;
		VMarg /= 2;
		D = 480+VMarg;
		U = -VMarg;
		if (std::min(m_S[YY],m_E[YY]) > D)
		{
			m_S[YY] -= m_pMyRoom->m_Size[YY];
			m_E[YY] -= m_pMyRoom->m_Size[YY];
			CalcMe();
		}
		if (std::max(m_S[YY],m_E[YY]) < U)
		{
			m_S[YY] += m_pMyRoom->m_Size[YY];
			m_E[YY] += m_pMyRoom->m_Size[YY];
			CalcMe();
		}
	}

	//glColor3f(0.f,1.f,0.f);
	//MGR->drawLine(m_S,m_E);
}

void cSegment::RenderMe()
{
	glColor3f(0.f,1.f,0.f);
	MGR->drawLine(m_S,m_E);
}

//========= EPIC GM TEST ==========
//struct gm_seg_s
//{
//	float sx;
//	float sy;
//	float ex;
//	float ey;
//	float nx;
//	float ny;
//	float dist;
//};
//float point_distance(float x1, float y1, float x2, float y2)
//{
//	return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
//}

//void cSegment::TraceAgainstMe_GM(const cSegment &ray, traceresult_s *ptr)
//{
//	//cSegment *seg1 = &ray, *seg2;
//	//seg1 = argument0; //ray
//	//seg2 = argument1; //obstacle
	
//	gm_seg_s seg1, seg2;
	
//	seg1.sx   = ray.m_S.x();
//	seg1.sy   = ray.m_S.y();
//	seg1.ex   = ray.m_E.x();
//	seg1.ey   = ray.m_E.y();
//	seg1.nx   = ray.m_N.x();
//	seg1.ny   = ray.m_N.y();
//	seg1.dist = ray.m_Dist;
	
//	seg2.sx   = m_S.x();
//	seg2.sy   = m_S.y();
//	seg2.ex   = m_E.x();
//	seg2.ey   = m_E.y();
//	seg2.nx   = m_N.x();
//	seg2.ny   = m_N.y();
//	seg2.dist = m_Dist;
	
//	float crossx = seg1.ex;
//	float crossy = seg1.ey;
//	int   crossed = 0;
	
//	if ((seg1.ex-seg1.sx)*seg2.nx+(seg1.ey-seg1.sy)*seg2.ny > 0)
//	{
//	   return;
//	}
	
//	float seg1s_dist, seg1e_dist, seg2s_dist, seg2e_dist;
//	seg1s_dist = (seg1.sx*seg2.nx+seg1.sy*seg2.ny);
//	seg1e_dist = (seg1.ex*seg2.nx+seg1.ey*seg2.ny);
	
//	float ofs;
//	ofs = 0.5;
	
//	if (fabs(seg1s_dist - seg2.dist) < ofs && fabs(seg1e_dist - seg2.dist) < ofs)
//	{
//		QVector2D t = m_T;
//		t.normalize();
//		float raydists;
		
//		raydists = QVector2D::dotProduct(ray.m_S-m_S,t);
		
//		if (raydists < -TRACE_EPSILON)
//			return;

//		if (raydists > (m_E-m_S).length()+TRACE_EPSILON)
//			return;

//	   crossx = seg1.ex-seg2.nx*(seg1e_dist - seg2.dist);
//	   crossy = seg1.ey-seg2.ny*(seg1e_dist - seg2.dist);
//	   crossed = 1<<0;
		
//		QVector2D cross(crossx,crossy);
//		ptr->UpdateResult(
//					cross,
//					(ray.m_S - cross).length()/(ray.m_S - ptr->endpos).length(),
//					m_N,this);
		
//	   return;
//	}
	
//	float w;
//	w = 0.5;
	
//	if (fabs(seg1s_dist - seg2.dist) < w && UTIL_Sign(seg1s_dist - seg2.dist) == UTIL_Sign(seg1e_dist - seg2.dist))
//	{
//	   float tx,ty;
//	   tx = seg2.ex - seg2.sx;
//	   ty = seg2.ey - seg2.sy;
//	   float l;
//	   l = point_distance(0,0,tx,ty);
//	   l = 1/l;
//	   tx *= l;
//	   ty *= l;
//	   float ds1,ds2,de2;
//	   ds1 = seg1.sx*tx+seg1.sy*ty;
//	   ds2 = seg2.sx*tx+seg2.sy*ty;
//	   de2 = seg2.ex*tx+seg2.ey*ty;
//	   if ((ds2 <= ds1+w && ds1-w <= de2) ||
//	       (ds2 >= ds1-w && ds1+w >= de2))
//	   {
//	      crossx = seg1.sx-seg2.nx*(seg1s_dist - seg2.dist);
//	      crossy = seg1.sy-seg2.ny*(seg1s_dist - seg2.dist);
//	      crossed = 1<<1;
			
//			QVector2D cross(crossx,crossy);
//			ptr->UpdateResult(
//						cross,
//						(ray.m_S - cross).length()/(ray.m_S - ptr->endpos).length(),
//						m_N,this);
			
//	      return;
//	   }
//	}
	
//	if ((seg1s_dist <= seg2.dist+w && seg2.dist-w <= seg1e_dist) ||
//	    (seg1s_dist >= seg2.dist-w && seg2.dist+w >= seg1e_dist))
//	{
//	   seg2s_dist = seg2.sx*seg1.nx+seg2.sy*seg1.ny;
//	   seg2e_dist = seg2.ex*seg1.nx+seg2.ey*seg1.ny;
//	   if ((seg2s_dist <= seg1.dist+w && seg1.dist-w <= seg2e_dist) ||
//	       (seg2s_dist >= seg1.dist-w && seg1.dist+w >= seg2e_dist))
//	   {
//	      float f;
//	      f  = fabs(seg1s_dist - seg1e_dist);
//	      if (f < 1) f = 1;
//	      //f2 = abs(seg2s_dist - seg2e_dist);
//	      //if (f2 < 1) f2 = 1;
//	      if (f >= 1)// && f2 >= 1)
//	      {
//	         f  = fabs(seg1s_dist - seg2.dist)/f;
//	        // f2 = abs(seg2s_dist - seg1.dist)/f2;
//	         crossx = seg1.sx*(1-f)+seg1.ex*(f);
//	         crossy = seg1.sy*(1-f)+seg1.ey*(f);
//	         float dist;
//	         dist = point_distance(seg1.sx,seg1.sy,crossx,crossy);
//	         if (dist > point_distance(seg1.sx,seg1.sy,seg1.ex,seg1.ey))
//	         {
//	            crossx = seg1.ex;
//	            crossy = seg1.ey;
//	         }
//				QVector2D cross(crossx,crossy);
//				ptr->UpdateResult(
//							cross,
//							(ray.m_S - cross).length()/(ray.m_S - ptr->endpos).length(),
//							m_N,this);
//	         crossed = 1<<2;
//	      }
//	   }
//	}
//}
extern bool g_bFrameAdvance;
extern bool g_bDebugPrint;
void cSegment::TraceAgainstMe(cSegment &ray, traceresult_s *ptr)
{
	QVector2D cross = ptr->endpos;
	
	if (ray.m_T.x()*m_N.x() + ray.m_T.y()*m_N.y() >= 0)
	{
		return;
	}
	
	
	
//	QVector2D ray_m_s = ray.m_S - (ray.m_E - ray.m_S).normalized()*TRACE_EPSILON;
	
//	float 
//			x1 = ray_m_s.x(),
//			x2 = ray.m_E.x(),
//			x3 = m_S.x(),
//			x4 = m_E.x(),
			
//			y1 = ray_m_s.y(),    
//			y2 = ray.m_E.y(),    
//			y3 = m_S.y(),
//			y4 = m_E.y();
	
//	float znam = (y4-y3)*(x2-x1) - (x4-x3)*(y2-y1);
	
//	if (znam == 0.f) return;
	
//	float f = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3))/znam;
	
//	float F_ERROR = FRAC_EPSILON/(ray.m_E - ray.m_S).length();
	
//	if (f  < 0.f-F_ERROR || f  > 1.f+F_ERROR) return;
	
//	float fb = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3))/znam;
	
//	float FB_ERROR = FRAC_EPSILON/(m_E - m_S).length();
	
//	if (fb < 0.f-FB_ERROR || fb > 1.f+FB_ERROR) return;
	
//	//f -= TRACE_EPSILON;
	
//	cross = ray_m_s + (ray.m_E - ray_m_s)*f;
//	cross -= (ray.m_E - ray.m_S).normalized()*TRACE_EPSILON;
//	ptr->UpdateResult(cross,f,m_N,this);
	
	float rays_dist, raye_dist, walls_dist, walle_dist;
	
	walls_dist = QVector2D::dotProduct(m_S,ray.m_N)-ray.m_Dist;
	walle_dist = QVector2D::dotProduct(m_E,ray.m_N)-ray.m_Dist;
	
	if (walls_dist >= 0.f && walle_dist >= 0.f) return;
	if (walls_dist <= 0.f && walle_dist <= 0.f) return;
	
	rays_dist = QVector2D::dotProduct(ray.m_S,m_N)-m_Dist;
	raye_dist = QVector2D::dotProduct(ray.m_E,m_N)-m_Dist;
	
//	if (rays_dist < -TRACE_EPSILON && rays_dist > -16.f && raye_dist < -TRACE_EPSILON) //stuck behind wall
//	{
//		ptr->UpdateResult(ray.m_S,0.f,m_N,this);
//		glDisable(GL_TEXTURE_2D);
//		glColor3f(1.f,0.f,0.f);
//		MGR->drawLine(m_S,m_E);
//		qDebug()<<"Stuck!";
//		return;
//	}
	
	//rays_dist -= m_Dist;
	//raye_dist -= m_Dist;
	
	if (rays_dist >= 0.f && raye_dist >= 0.f) return;
	if (rays_dist <= 0.f && raye_dist <= 0.f) return;
	
	float f;
	
	if (rays_dist < 0.0f)
		f = ( rays_dist + TRACE_EPSILON ) / ( rays_dist - raye_dist );
	else
		f = ( rays_dist - TRACE_EPSILON ) / ( rays_dist - raye_dist );

	f = std::max(f,0.f);
	f = std::min(f,1.f);
	cross = ray.m_S + (ray.m_E - ray.m_S)*f;
	ptr->UpdateResult(cross,f,m_N,this);

	
//	QVector2D ray_m_s = ray.m_S;
	
//	float rays_dist, raye_dist, walls_dist, walle_dist;
//	rays_dist = QVector2D::dotProduct(ray_m_s,m_N);
//	raye_dist = QVector2D::dotProduct(ray.m_E,m_N);
	
//	float raydists_n;
//	raydists_n = QVector2D::dotProduct(ray_m_s-m_S,m_N);
	
//	if (raydists_n < 0.f/*TRACE_EPSILON*2.f*/ && raydists_n > -TRACE_EPSILON*2.f)
//	{
//		QVector2D t = m_T;
//		t.normalize();
//		float raydists;
//		raydists = QVector2D::dotProduct(ray_m_s-m_S,t);
		
//		ray_m_s = m_S+t*(raydists)/*+m_N*TRACE_EPSILON*/;
		
//		rays_dist = QVector2D::dotProduct(ray_m_s,m_N);
//		//qDebug()<<"corrected";
//	}
//	float ofs;
//	ofs = TRACE_EPSILON;//*3.f;
	
//	if (fabs(rays_dist - m_Dist) < ofs && fabs(raye_dist - m_Dist) < ofs)
//	{
//		QVector2D t = m_T;
//		t.normalize();
//		float raydists;
		
//		raydists = QVector2D::dotProduct(ray_m_s-m_S,t);
		
//		if (raydists < -TRACE_EPSILON)
//		{
//			return;
//		}
//		if (raydists > (m_E-m_S).length()+TRACE_EPSILON)
//		{
//			return;
//		}
		
//		float d = QVector2D::dotProduct(ray.m_E - ray_m_s,t);
//		cross = 
//				//ray_m_s+t*d;
//				ray.m_E-m_N*(raye_dist - m_Dist);
//		float frac = ray.m_T.length();//(ray.m_S - ptr->endpos).length();
//		if (frac != 0.f) frac = (ray_m_s - cross).length()/frac; else frac = 0.f;
//		ptr->UpdateResult(cross,frac,m_N,this);
//		return;
//	}
	
//	float w;
//	w = TRACE_EPSILON;//3.f;
	
//	if (fabs(rays_dist - m_Dist) < w && UTIL_Sign(rays_dist - m_Dist) == UTIL_Sign(raye_dist - m_Dist))
//	{
//		QVector2D t = m_T;
//		t.normalize();
//		float ds1,ds2,de2;
//		ds1 = QVector2D::dotProduct(ray_m_s,t);
//		ds2 = QVector2D::dotProduct(m_S,t);
//		de2 = QVector2D::dotProduct(m_E,t);
//		if ((ds2 <= ds1+w && ds1-w <= de2) ||
//			 (ds2 >= ds1-w && ds1+w >= de2))
//		{
//			cross = ray_m_s-m_N*(rays_dist - m_Dist);
//			float frac = ray.m_T.length();//(ray.m_S - ptr->endpos).length();
//			if (frac != 0.f) frac = (ray_m_s - cross).length()/frac; else frac = 0.f;
//			ptr->UpdateResult(cross,frac,m_N,this);
//			return;
//		}
//	}
//	if ((rays_dist <= m_Dist+w && m_Dist-w <= raye_dist) ||
//		 (rays_dist >= m_Dist-w && m_Dist+w >= raye_dist))
//	{
//		walls_dist = QVector2D::dotProduct(m_S,ray.m_N);
//		walle_dist = QVector2D::dotProduct(m_E,ray.m_N);
//		//qDebug()<<walls_dist<<walls_dist<<m_Dist;
		
//		if ((walls_dist <= ray.m_Dist+w && ray.m_Dist-w <= walle_dist) ||
//			 (walls_dist >= ray.m_Dist-w && ray.m_Dist+w >= walle_dist))
//		{
//			float f;
//			f  = fabs(rays_dist - raye_dist);
//			if (f < 1) f = 1;
//			if (f >= 1)
//			{
//				f  = (abs(rays_dist - m_Dist)/*- TRACE_EPSILON*/)/f;
//				f = std::max(f,0.f);
//				cross = ray_m_s*(1-f)+ray.m_E*(f);
//				if ((ray_m_s - cross).length() > ray.m_T.length())
//					cross = ray.m_E;
//				float errorDist = QVector2D::dotProduct(cross - m_S,m_N);
//				if (errorDist < 0.f && errorDist >= -TRACE_EPSILON)
//				{
//					cross += m_N*(errorDist+TRACE_EPSILON);
//				}
//				float frac = ray.m_T.length();//(ray.m_S - ptr->endpos).length();
//				if (frac != 0.f) frac = (ray_m_s - cross).length()/frac; else frac = 0.f;
//				ptr->UpdateResult(cross,frac,m_N,this);
//			}
//		}
//	}
}
