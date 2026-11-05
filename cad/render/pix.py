# pix.py - a tiny software renderer for val_v1_render.py: projects triangles for a view
# (elevation, azimuth), z-buffers them into a pixel grid, then colours each part from a
# palette with three light levels and dark outlines (16-bit style).
import numpy as np
def rot(el,az):
    e,a=np.radians(el),np.radians(az)
    d=np.array([np.cos(e)*np.cos(a),np.cos(e)*np.sin(a),np.sin(e)])
    up=np.array([0,0,1.]) if abs(el)<89 else -np.array([-np.sin(a),np.cos(a),0])
    r=np.cross(up,d); r/=np.linalg.norm(r); u=np.cross(d,r)
    return np.stack([r,u,d])
def render(parts,el,az,scale,pad=4,bounds=None):
    mats=sorted(set(m for _,m in parts)); mid={m:i+1 for i,m in enumerate(mats)}
    T=np.concatenate([t for t,_ in parts]); M=np.concatenate([np.full(len(t),mid[m]) for t,m in parts])
    R=rot(el,az); P=T@R.T
    nrm=np.cross(T[:,1]-T[:,0],T[:,2]-T[:,0]); nrm/=np.linalg.norm(nrm,axis=1,keepdims=True)+1e-12
    # light from upper-left-front of the viewer
    L=R[2]*0.5+R[1]*0.7-R[0]*0.5; L/=np.linalg.norm(L)
    vis=np.abs(nrm@R[2]); sh=np.abs(nrm@L)
    mn,mx=(bounds if bounds is not None else (P[:,:,:2].reshape(-1,2).min(0),P[:,:,:2].reshape(-1,2).max(0)))
    W,H=(np.ceil((mx-mn)*scale)).astype(int)+2*pad
    ids=np.zeros((H,W),int); zb=np.full((H,W),-1e9); shade=np.zeros((H,W))
    X=(P[:,:,0]-mn[0])*scale+pad; Y=H-((P[:,:,1]-mn[1])*scale+pad); Z=P[:,:,2]
    for i in range(len(T)):
        x,y,z=X[i],Y[i],Z[i]
        x0,x1=int(max(np.floor(x.min()),0)),int(min(np.ceil(x.max()),W-1))
        y0,y1=int(max(np.floor(y.min()),0)),int(min(np.ceil(y.max()),H-1))
        if x1<x0 or y1<y0: continue
        gx,gy=np.meshgrid(np.arange(x0,x1+1)+.5,np.arange(y0,y1+1)+.5)
        d=(y[1]-y[2])*(x[0]-x[2])+(x[2]-x[1])*(y[0]-y[2])
        if abs(d)<1e-12: continue
        a=((y[1]-y[2])*(gx-x[2])+(x[2]-x[1])*(gy-y[2]))/d
        b=((y[2]-y[0])*(gx-x[2])+(x[0]-x[2])*(gy-y[2]))/d
        c=1-a-b; m=(a>=-1e-3)&(b>=-1e-3)&(c>=-1e-3)
        if not m.any(): continue
        zz=a*z[0]+b*z[1]+c*z[2]
        sub=zb[y0:y1+1,x0:x1+1]; m&=zz>sub
        sub[m]=zz[m]; ids[y0:y1+1,x0:x1+1][m]=M[i]; shade[y0:y1+1,x0:x1+1][m]=sh[i]
    return ids,shade,zb,{v:k for k,v in mid.items()}

def hexrgb(h): return np.array([int(h[i:i+2],16) for i in (1,3,5)],float)
def colorize(ids,shade,zb,names,pal,scale):
    H,W=ids.shape; img=np.zeros((H,W,4),np.uint8)
    for k,name in names.items():
        m=ids==k
        if name.startswith('led_') and 'led' in pal and callable(pal['led']):
            col=hexrgb(pal['led'](name)); img[m,:3]=col; img[m,3]=255; continue
        base=hexrgb(pal[name])
        t=shade[m]
        lvl=np.where(t>0.75,1.12,np.where(t>0.45,0.92,0.7))
        img[m,:3]=np.clip(base*lvl[:,None],0,255); img[m,3]=255
    # outlines: silhouette and part boundaries / depth jumps
    occ=ids>0
    edge=np.zeros_like(occ)
    for dy,dx in ((0,1),(1,0),(0,-1),(-1,0)):
        n_ids=np.roll(np.roll(ids,dy,0),dx,1); n_z=np.roll(np.roll(zb,dy,0),dx,1)
        edge|= occ & ((n_ids!=ids) & (n_z< zb - 0.01) | (n_z < zb - 6.0))
    sil=occ & ~(np.roll(occ,1,0)&np.roll(occ,-1,0)&np.roll(occ,1,1)&np.roll(occ,-1,1))
    dark=hexrgb(pal['outline'])
    for name_k,name in names.items():
        m=edge&(ids==name_k)&~sil
        if name.startswith('led_') and 'led' in pal and callable(pal['led']): continue
        img[m,:3]=np.clip(hexrgb(pal[name])*0.45,0,255)
    img[sil,:3]=dark
    return img
def upscale(img,k): return img.repeat(k,0).repeat(k,1)
