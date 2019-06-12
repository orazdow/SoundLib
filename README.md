#### SoundLib

SoundLib is an environment for live audio programming and creating plugins and software. Currently it's a library of dsp and control objects being built into an interpreted environment, SoundLib is just a working title. 

The main audio base-class **Sig** should provide everything needed to incorporate classes from other libraries like Stk or Maximilian:
```
:::Sig:::

VARIABLES

float* inputs[num_inlets]  --(default is 64)
float* input --(same as inputs[0])
float output
float val --provided to store 1 intial value

FUNCTIONS:

--init--
void init(uint _inlets = 1, bool _summing = auto_summing)
    -sets number of active inlets, toggles summing behavior
     Each inlet is itself a summing-bus with a max of 64 parralel inputs. 
     This can be overriden in which case each inlet is mapped directly to the summing input, 
     otherwise each of the 64 inlets (inputs*) is mapped to a summing output.

void initVal(float* val, int inlet)
    -points an inlet to any existing float
void setVal(float f, int inlet = 1)
    -assigns a value to val and points an inlet to it

--constructors--
Sig(double val = 0) 
    -(truncates to float)
Sig(uint inlets, bool summing) 
    -construct with n inlets, summing

--dsp--
virtual void dsp()
    -this is where a child class should implement it's dsp routine

virtual float out()
virtual float out(double in)
    -these can be implemented to add dsp called by the user instead of being called by the library
    
--connecting--
void connect(Sig* child, uint inlet = 0) 
        -adds a child object to the Sig graph, disconnecting it from the root node, 
         and connect's a's output to b's input at a specified inlet.
void disconnect(Sig* child, uint inlet = 0) 
        -disconnects, and reconnects b to the root node if neccesary.
        
        -these call external sig_connect/sig_disconect functions which also include 
         sig_connect(float* a, Sig* b, uint inlet = 0)
         which will connect (or remove) any float* to an inlet. 

--others--
void Call()
        -this is calls dsp() and sumInputs() if summing, and is called by the dsp scheduler
virtual void bypass_summing(uint inlet = 0)
        -this called by Connect/disconnect
void sumInputs()

===================
--Here is an example of a Sig object, 2-inlet sum (like +~ in PD)

class Sum : public Sig{
    public:
        Sum(double f = 0){
            init(2); // 2 inlets
            setVal(f, 1); // set default val to right inlet
        }
        
        void dsp(){ 
            output = *inputs[0];
            output += *inputs[1];
        }
};
===================
```
---
The base class for control objects is **Ctl**. Control messages have a simple structure currently and all message sources are polling to generate messages, so events and a different message protocol of some kind will be incorporated. 

```
:::Ctl:::

Msg struct:
    --depending on protocals like midi, osc or fudi, and how parameters are parsed this will likely change.
struct Note{
    unsigned short note;
    uint8_t vel; 
    bool on; 
    short pitch; 
};

union Val{
    uint32_t _i;
    float _f; 
    Note _n;
};

struct Msg{
    Val* value;
    uint num = 0;
    uint index = 0;
    uint8_t type = 0;
};

Variables:
Msg m;

Functions:

--init--
void msg_alloc(size_t num)
        --this must be called to allocate space for the contents of a message

--control--
virtual void run(Msg _m) 
virtual void run()
        -these are the control equivalent of dsp() in Sig.
        
--others--
void copy_msg(Msg _m)
        -copy the contents of an incoming message
virtual void onConnect(Ctl* child)
        -optional onConnect callback
```
---
**Sig** objects are in ```soundlib_sig.h``` and ```expr.h```, which is an audio expression parser (like pd's expr~) with the variables f0-f63 corresponding to it's inlets. **Ctl** objects are in ```soundlib_ctl.h```.

```
Todo:
    --Implement MIDI, events
    --Implement proper scheduler
    --Implement lanhuage/interpreter 
    --[expr...]->Obj like syntax
    --oversampling
    --delay objects
    --integrate VST api
```
   