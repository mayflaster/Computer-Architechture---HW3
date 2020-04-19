/* 046267 Computer Architecture - Winter 2019/20 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */
#include <algorithm>
#include "dflow_calc.h"
#include <iostream> ///
using namespace std;
class ProgAnalyzer{
public:
    unsigned int *opsLatency;
    InstInfo *progTrace;
    unsigned int numOfInsts;
    int *max; // array of the max latency to each command
    bool *isexit;//no one has deppendency to this command
    ProgAnalyzer(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts){
      this->numOfInsts = numOfInsts;
      this->isexit = new bool[this->numOfInsts];
      this->max = new int[this->numOfInsts];
      this->opsLatency = new unsigned int[this->numOfInsts];
      this->progTrace = new InstInfo[this->numOfInsts];
      for(int i=0;i<(int)numOfInsts;i++){
         //this-> isexit[i] = false;
          this->progTrace[i] = progTrace[i];
          this->opsLatency[i]=opsLatency[i];
      }
      for(int j=0; j<(int)numOfInsts; j++){ //the dests
          //initialize isexit arr
          this->isexit[j] = true;
          for(int k=j+1;k<(int)numOfInsts;k++){
              if (this->progTrace[j].dstIdx == this->progTrace[k].dstIdx){
                  this->isexit[j]=true;
                  break;
              }
          //for(int k=numOfInsts-1; k>j;k--){//the sources
              if((unsigned)this->progTrace[j].dstIdx == this->progTrace[k].src1Idx || (unsigned)this->progTrace[j].dstIdx == this->progTrace[k].src2Idx ){
                  this->isexit[j] = false; // this dest is a source of a higher command
                  break;
              }
          }
      }
      this->isexit[numOfInsts-1]=true;
        //initialize max array
        int source1latency=0;
        int source2latency=0;
        max[0]=0;
        for(int m=1; m<(int)numOfInsts ;m++){
            for (int a=m-1; a>=0;a--) {
                if ((unsigned)progTrace[a].dstIdx == progTrace[m].src1Idx) {
                    source1latency = max[a] + (int)(this->opsLatency[this->progTrace[a].opcode]);
                    break;
                }
            }
            for(int b=m-1;b>=0;b--){
                if((unsigned)progTrace[b].dstIdx == progTrace[m].src2Idx ){
                    source2latency=max[b]+ (int)(this->opsLatency[this->progTrace[b].opcode]);
                    break;
                }
            }
            this->max[m]= std::max(source1latency,source2latency);
          source1latency=0;
          source2latency=0;
      }
    }
    ~ProgAnalyzer(){
        delete [] this->isexit;
        delete [] this->progTrace;
        delete [] this->opsLatency;
        delete [] this->max;
    }
};

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    if(numOfInsts == 0)
        return PROG_CTX_NULL;
    ProgAnalyzer *progAnalyz = new ProgAnalyzer(opsLatency,progTrace,numOfInsts);
    return progAnalyz;
}

void freeProgCtx(ProgCtx ctx) {
    delete (ProgAnalyzer*)ctx;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    if(theInst > ((ProgAnalyzer*)ctx)->numOfInsts)
        return -1;
   else
       return ((ProgAnalyzer*)ctx)->max[theInst];
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    ProgAnalyzer* progCtx = (ProgAnalyzer*)ctx;
    *src1DepInst=-1;
    *src2DepInst=-1;
   if(theInst > progCtx->numOfInsts )
        return -1;
   if(theInst==0){
       return 0;
   }
    for (unsigned int  i = theInst; i>0 ; --i){
        if ((unsigned)progCtx->progTrace[i-1].dstIdx==progCtx->progTrace[theInst].src1Idx){
            *src1DepInst=(int)(i-1);
            break;
        }
    }
    for (unsigned int j = theInst; j>0 ;--j){
        if ((unsigned)progCtx->progTrace[j-1].dstIdx==progCtx->progTrace[theInst].src2Idx){
            *src2DepInst=(int)(j-1);
            break;
        }
    }
    return 0;
}

int getProgDepth(ProgCtx ctx) {
    ProgAnalyzer* progCtx = (ProgAnalyzer*)ctx;
    int maxLengthToExit=0;
    for(int i=0;i<(int)(progCtx->numOfInsts);i++) {
        if (progCtx->isexit[i]){//if no one depends on this command
            if (((progCtx->max[i] + (int) progCtx->opsLatency[progCtx->progTrace[i].opcode])) > maxLengthToExit) {
                maxLengthToExit = progCtx->max[i] + (int) progCtx->opsLatency[progCtx->progTrace[i].opcode];

            }
    }
    }

    return maxLengthToExit;
}


