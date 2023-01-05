
myfont=[]

font=open("../resources/RATA.fnt", "rb" )
while (byte:=font.read(1)):
    myfont.append(byte)
for nf in range(1,2):
    frames=[[],[]]
    f=open(f"../assets/rata_strip{nf}.bmp", "rb" )
    i=0
    idx=0
    while (byte:=f.read(1)):
        frames[i].append(byte)
    frames[i]=frames[i][32:]
    j=8*64
    for frame in frames:
        for b in frame:
            myfont[j]=b
            j+=1;

        
dest_font=open("../resources/final.fnt", "wb" )
for b in myfont:
    dest_font.write(b)


