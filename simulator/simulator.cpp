#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <typeinfo>
#include <cstdint>
#include <chrono>
#include <thread>
#include <functional>
#include <sstream>
#include <atomic>


class Variable {
    private: 
        int address;
        std::string name;
        std::string type;

    public:
        Variable (int address , const std::string& name, const std::string& type) : address(address), name(name) ,type(type) {}

        int getAddress() {
            return address;
        }

        const std::string& getName() const {
            return name;
        }

        const std::string& getType() const {
            return type;
        }

};

class VarVector {
    private:
        std::vector<Variable> vars;

    public:
        VarVector() = default;

        std::vector<Variable>& getVars() {
            return vars;
        }

        
        void setVariables(std::string& filename) {
            std::ifstream file(filename);

            if(!file.is_open()) {
                std::cerr << "File couldnt be open: " << filename << "\n";
                return;
            }

            std::string line;

            while(std::getline(file, line)) {
                std::istringstream iss(line);
                std::string addrStr, objCode, label, directive, operand;

                if(!(iss >> addrStr)) continue;
                int addr = std::stoi(addrStr, nullptr, 16);

                iss >> objCode >> label >> directive >> operand;

                if(directive == "WORD" || directive == "RESW") {
                    vars.emplace_back(addr, label, "WORD");
                } else if( directive == "BYTE" || directive == "RESB") {
                    vars.emplace_back(addr, label, "BYTE");
                }

                //std::cout << line << "\n";
            }
        }
};

class Register {
    private:
        int index;
        int value;

    public: 
        Register (int ix, int val = 0) : index(ix) ,value(val) {}

        void setReg(int v) {
            value = v;
        }

        int getValue() const {
            return value;
        }

        int getIndex() const {
            return index;
        }
};

class Memory {
    private:
        std::vector<uint8_t> memory;
    public:
        Memory() : memory(1048576, 0) {}

        uint8_t getByte(int addr) const {
            return memory.at(addr);
        }

        void setByte(int addr, uint8_t val) {
            memory.at(addr) = val;
        }

        int getWord(int addr) const {
            return (memory.at(addr) << 16) | (memory.at(addr + 1) << 8) | (memory.at(addr+2)); 
        }

        void setWord(int addr, int val) {
            memory.at(addr) = (val >> 16) & 0xFF;
            memory.at(addr+1) = (val >> 8) & 0xFF;
            memory.at(addr+2) = val & 0xFF;
        }

        void clear() {
            std::fill(memory.begin(), memory.end(), 0);
        }
};

class Device {
    public:
        virtual ~Device() = default; // če ni definiran desktruktor se otrok deodvanega razreda v tem primeru InputDevice

        virtual bool test() {return true;}
        virtual uint8_t read() {return 0;}
        virtual void write(uint8_t value) {}
};

class InputDevice : public Device {
    private:
        std::istream& in;
    
    public:
        InputDevice(std::istream& input) : in(input) {}

        uint8_t read() override {
            char c;
            if(in.get(c)) {
                return static_cast<uint8_t>(c);
            }

            return 0xFF;
        }

};

class OutputDevice : public Device {
    private:
        std::ostream& out;

    public:
        OutputDevice(std::ostream& output) : out(output) {}

        void write(uint8_t value) override {
            out.put(static_cast<char>(value));
        }
};

class FileDevice : public Device {
    private:
        std::fstream file;
        std::string filename;

    public:
        FileDevice(const std:: string& name) : filename(name) {}

        ~FileDevice() {
            if(file.is_open()) {
                file.close();
            }
        }

        bool test() override {
            file.open(filename, std::ios::binary);

            if(file.is_open()) {
                file.close();
                return true;
            } else {
                throw std::runtime_error("Input fule not found: " + filename);
            }
        }

        uint8_t read() override {
            file.open(filename, std::ios::in| std::ios::binary);

            if(file.is_open()) {
                char c;
                if(file.get(c)) {
                    return static_cast<uint8_t>(c);
                }    
                
                file.close();
                return 0xFF;
            } else {
                throw std::runtime_error("Input fule not found: " + filename);
            }
        }

        void write(uint8_t value) override {
            file.open(filename, std::ios::binary | std::ios::out);

            if(file.is_open()) {
                file.put(static_cast<char>(value));
                file.flush();
                file.close();
            } else {
                throw std::runtime_error("Input fule not found: " + filename);
            }
        }



    
};

class Machine {
    public:
        //inicializacija registrov
        Register A = Register(0,0);
        Register X = Register(1,0);
        Register L = Register(2,0);
        Register B = Register(3,0);
        Register S = Register(4,0);
        Register T = Register(5,0);
        Register F = Register(6,0);
        Register PC = Register(7,0);
        Register SW = Register(8,0);
        bool stopFlag = true;

        //inicializacija spomina
        Memory memory;

        //inicializacija seznamma naprav
        std::vector<std::unique_ptr<Device>> devices;
        Machine() : devices(256) {
            devices[0] = std::make_unique<InputDevice>(std::cin);
            devices[1] = std::make_unique<OutputDevice>(std::cout);
            devices[2] = std::make_unique<OutputDevice>(std::cerr);
        }

        Device* getDevice(int num) {
            return devices[num].get();
        }

        void setDevice(int num, std::unique_ptr<Device> device) {
          //devices.at(num) = device;
          if(num >= 3 && typeid(*device) == typeid(FileDevice)){
            devices[num] = std::move(device);
          } else if(num < 3) {
            devices[num] = std::move(device);
          }
        }

        void reset() {
            A.setReg(0);
            X.setReg(0);
            L.setReg(0);
            B.setReg(0);
            S.setReg(0);
            T.setReg(0);
            F.setReg(0);
            PC.setReg(0);
            SW.setReg(0);

            memory.clear();
        }
            



};

void showRegisters(Machine& machine) {
    std::cout << "Registers:\n";
    std::cout << " A=" << std::hex << machine.A.getValue()
    << " X=" << machine.X.getValue()
    << " L=" << machine.L.getValue()
    << " B=" << machine.B.getValue()
    << " S=" << machine.S.getValue()
    << " T=" << machine.T.getValue()
    << " F=" << machine.F.getValue()
    << " PC=" << machine.PC.getValue()
    << " SW=" << machine.SW.getValue() << "\n";

}

void showInstructions(Machine& machine, const std::string& mnemonic, int addr, int operand, int before, int after, const std::string& reg) {
    std::cout << "[PC=]" << std::hex << machine.PC.getValue() << "]"
    << mnemonic << " @" << std::hex << addr << "(operand=" << operand << ")"
    <<"| " << reg << ": " << before << " -> " << after << "\n";
}

void showStore(Machine& machine, const std::string& mnemonic, int addr, int val) {
    std::cout << "[PC=" << std::hex << machine.PC.getValue() << "] " 
    << mnemonic << " -> Memory[" << std::hex << addr << "] = " << val << "\n";
}


void showVariables(VarVector& vec, Memory& memory) {
    std::cout << "Variables:\n";
    std::cout << " Addr   Name     Type   Value\n";

    for(auto& v : vec.getVars()) {
        int val = (v.getType() == "WORD") ? memory.getWord(v.getAddress()) : memory.getByte(v.getAddress());
        std::cout << " " << std::hex << v.getAddress() << "   " << v.getName()
        << "   " << v.getType() << "   " << val << "\n";

    }
}


class Opcode {
    public:
        static constexpr int ADD = 0x18;
        static constexpr int ADDF = 0x58;
        static constexpr int ADDR = 0x90;
        static constexpr int AND = 0x40;
        static constexpr int CLEAR = 0xB4;
        static constexpr int COMP = 0x28;
        static constexpr int COMPF = 0x88;
        static constexpr int COMPR = 0xA0;
        static constexpr int DIV = 0x24;
        static constexpr int DIVF = 0x64;
        static constexpr int DIVR = 0x9C;
        static constexpr int FIX = 0xC4;
        static constexpr int FLOAT = 0xC0;
        static constexpr int HIO = 0xF4;
        static constexpr int J = 0x3C;
        static constexpr int JEQ = 0x30;
        static constexpr int JGT = 0x34;
        static constexpr int JLT = 0x38;
        static constexpr int JSUB = 0x48;
        static constexpr int LDA = 0x00;
        static constexpr int LDB = 0x68;
        static constexpr int LDCH = 0x50;
        static constexpr int LDF = 0x70;
        static constexpr int LDL = 0x08;
        static constexpr int LDS = 0x6C;
        static constexpr int LDT = 0x74;
        static constexpr int LDX = 0x04;
        static constexpr int LPS = 0xD0;
        static constexpr int MUL = 0x20;
        static constexpr int MULF = 0x60;
        static constexpr int MULR = 0x98;
        static constexpr int NORM = 0xC8;
        static constexpr int OR = 0x44;
        static constexpr int RD = 0xD8;
        static constexpr int RMO = 0xAC;
        static constexpr int RSUB = 0x4C;
        static constexpr int SHIFTL = 0xA4;
        static constexpr int SHIFTR = 0xA8;
        static constexpr int SIO = 0xF0;
        static constexpr int SSK = 0xEC;
        static constexpr int STA = 0x0C;
        static constexpr int STB = 0x78;
        static constexpr int STCH = 0x54;
        static constexpr int STF = 0x80;
        static constexpr int STI = 0xD4;
        static constexpr int STL = 0x14;
        static constexpr int STS = 0x7C;
        static constexpr int STSW = 0xE8;
        static constexpr int STT = 0x84;
        static constexpr int STX = 0x10;
        static constexpr int SUB = 0x1C;
        static constexpr int SUBF = 0x5C;
        static constexpr int SUBR = 0x94;
        static constexpr int SVC = 0xB0;
        static constexpr int TD = 0xE0;
        static constexpr int TIO = 0xF8;
        static constexpr int TIX = 0x2C;
        static constexpr int TIXR = 0xB8;
        static constexpr int WD = 0xDC;



};



class Executor {
    
    Machine& machine;
    VarVector& vars;
    bool ended = false;

    public: 
        Executor(Machine& mac, VarVector& v) : machine(mac), vars(v) {}


        uint8_t fetch() {
            int addr = machine.PC.getValue();
            uint8_t byte = machine.memory.getByte(addr);
            addr = addr + 1;
            machine.PC.setReg(addr);
            return byte;
        }

        Register& findReg(int ix) {
            switch(ix) {
                case 0: return machine.A;
                case 1: return machine.X;
                case 2: return machine.L;
                case 3: return machine.B;
                case 4: return machine.S;
                case 5: return machine.T;
                case 6: return machine.F;
            }
            return machine.A;
        } 

        int getAddr(uint8_t ni, uint8_t opcode) {
            int addr = 0;
            int value = 0;
            
            uint8_t flags = fetch();
            uint8_t disp1 = flags & 0x0F;
            bool x = (flags >> 7) & 1;
            bool b = (flags >> 6) & 1;
            bool p = (flags >> 5) & 1;
            bool e = (flags >> 4) & 1;

            //check e
            if(e) {
                uint8_t disp2 = fetch();
                uint8_t disp3 = fetch();
                
                addr = (disp1 << 16) | (disp2 << 8) | (disp3); 
            } else {
                uint8_t disp2 = fetch();
                addr = (disp1 << 8) | (disp2);
            }

            //check p
            if(p) {

                if(addr & 0x800) {
                    addr |= 0xFFFFF000;
                }

                addr = machine.PC.getValue() + addr;
            }

            if(b) {
                addr = machine.B.getValue() + addr;
            }

            if(x) {
                addr = machine.X.getValue() + addr;
            }

            return addr;
        }

        int getValueFromMemory(uint8_t opcode, uint8_t ni, int addr) {
            int value = 0;
            bool i = ni & 1;
            bool n = (ni >> 1) & 1;
            
            if(opcode == Opcode::LDCH) {
                if(n == 1 && i == 0) {
                    value = machine.memory.getByte(machine.memory.getWord(addr));
                } else if(n == 0 && i == 1) {
                    value = addr;
                } else {
                    value = machine.memory.getByte(addr);
                }
            } else {
                if(n == 1 && i == 0) {
                    value = machine.memory.getWord(machine.memory.getWord(addr));
                } else if(n == 0 && i == 1) {
                    value = addr;
                } else {
                    value = machine.memory.getWord(addr);
                }
            }

            return value;
        }

        void setValueOnMemory(uint8_t opcode, uint8_t ni, int addr, int value) {
            bool i = ni & 1;
            bool n = (ni >> 1) & 1;
            
            if(opcode == Opcode::STCH) {
                if(n == 1 && i == 0) {
                    machine.memory.setByte(machine.memory.getWord(addr), value);
                } else if(n == 0 && i == 1) {
                    std::cerr << "Cannot do that!\n";
                } else {
                    machine.memory.setByte(addr, value);
                }
            } else {
                if(n == 1 && i == 0) {
                    machine.memory.setWord(machine.memory.getWord(addr), value);
                } else if(n == 0 && i == 1) {
                    std::cerr << "Cannot do that!\n";
                } else {
                    machine.memory.setWord(addr, value);
                }
            }
        
        }

        void execute() {
            //bool ended = false;
            if(ended) {return;}

            //while(!ended) {
                int beforePC = machine.PC.getValue();
                //std::cout << "Before value: " << beforePC << "\n";
                uint8_t op = fetch();
                uint8_t opcode = op & 0xFC;
                uint8_t ni = op & 0x03; 

                switch(opcode) {
                    case Opcode::FIX: break;
                    case Opcode::FLOAT: break;
                    
                    //register commands F2
                    case Opcode::ADDR: {
                        uint8_t registra = fetch(); 
                        uint8_t r1 = (registra >> 4) & 0x0F;
                        uint8_t r2 = registra & 0x0F;
                        int ix1 = int(r1);
                        int ix2 = int(r2);
                        Register& reg1 = findReg(ix1);
                        Register& reg2 = findReg(ix2);

                        int v1 = reg1.getValue();
                        int v2 = reg2.getValue();
                        reg2.setReg( v1 + v2);
                        showInstructions(machine, "ADDR", -1, v1, v2, reg2.getValue(), "R" + std::to_string(ix2));
                        break;}
                    case Opcode::CLEAR: {
                        uint8_t registra = fetch(); 
                        uint8_t r1 = (registra >> 4) & 0x0F;
                        //uint8_t r2 = registra & 0x0F;
                        int ix1 = int(r1);
                        //int ix2 = int(r2);
                        Register& reg1 = findReg(ix1);
                        //Register reg2 = findReg(ix2);

                        reg1.setReg(0);
                    
                        break;}
                    case Opcode::COMPR: { 
                        uint8_t registra = fetch(); 
                        uint8_t r1 = (registra >> 4) & 0x0F;
                        uint8_t r2 = registra & 0x0F;
                        int ix1 = int(r1);
                        int ix2 = int(r2);
                        Register& reg1 = findReg(ix1);
                        Register& reg2 = findReg(ix2);

                        int v1 = reg1.getValue();
                        int v2 = reg2.getValue();
                        
                        if(v1 < v2) {
                            machine.SW.setReg(-1);
                        } else if(v1 == v2) {
                            machine.SW.setReg(0);
                        } else {
                            machine.SW.setReg(1);
                        }
                        
                        break;}
                    case Opcode::DIVR: { 
                        uint8_t registra = fetch(); 
                        uint8_t r1 = (registra >> 4) & 0x0F;
                        uint8_t r2 = registra & 0x0F;
                        int ix1 = int(r1);
                        int ix2 = int(r2);
                        Register& reg1 = findReg(ix1);
                        Register& reg2 = findReg(ix2);

                        int v1 = reg1.getValue();
                        int v2 = reg2.getValue();
                        reg2.setReg( v2 / v1);
                        showInstructions(machine, "DIVR", -1, v2, v1, reg2.getValue(), "R" + std::to_string(ix2));
                        break;}
                    case Opcode::MULR: {
                        uint8_t registra = fetch(); 
                        uint8_t r1 = (registra >> 4) & 0x0F;
                        uint8_t r2 = registra & 0x0F;
                        int ix1 = int(r1);
                        int ix2 = int(r2);
                        Register& reg1 = findReg(ix1);
                        Register& reg2 = findReg(ix2);

                        int v1 = reg1.getValue();
                        int v2 = reg2.getValue();
                        reg2.setReg( v1 * v2);
                        showInstructions(machine, "MULR", -1, v1, v2, reg2.getValue(), "R" + std::to_string(ix2));
                        break;}
                    case Opcode::RMO: { 
                        uint8_t registra = fetch(); 
                        uint8_t r1 = (registra >> 4) & 0x0F;
                        uint8_t r2 = registra & 0x0F;
                        int ix1 = int(r1);
                        int ix2 = int(r2);
                        Register& reg1 = findReg(ix1);
                        Register& reg2 = findReg(ix2);

                        int v1 = reg1.getValue();
                        int v2 = reg2.getValue();

                        reg2.setReg(v1);

                        showInstructions(machine, "RMO", -1, v1, v2, reg2.getValue(), "R" + std::to_string(ix2));
                        break;}
                    case Opcode::SHIFTL: {
                        uint8_t registra = fetch();
                        uint8_t r = (registra >> 4) & 0x0F;
                        int n = (int)(registra & 0x0F);

                        int ix1 = int(r);
                        Register& reg = findReg(ix1);
                        std::cout << "n: " << n << "\n";

                        int before = reg.getValue();
                        int v = (before << n) & 0xFFFFFF;
                        reg.setReg(v);
                        showInstructions(machine, "SHIFTL", -1, n, before, reg.getValue(), "R" + std::to_string(ix1));

                        break;}
                    case Opcode::SHIFTR: {
                        uint8_t registra = fetch();
                        uint8_t r = (registra >> 4) & 0x0F;
                        int n = (int)(registra & 0x0F);

                        int ix1 = int(r);
                        Register& reg = findReg(ix1);

                        int before = reg.getValue();
                        int v = (before >> n) & 0xFFFFFF;
                        reg.setReg(v);
                        showInstructions(machine, "SHIFTL", -1, n, before, reg.getValue(), "R" + std::to_string(ix1));

                        break;}
                    case Opcode::SUBR: { 
                        uint8_t registra = fetch(); 
                        uint8_t r1 = (registra >> 4) & 0x0F;
                        uint8_t r2 = registra & 0x0F;
                        int ix1 = int(r1);
                        int ix2 = int(r2);
                        Register& reg1 = findReg(ix1);
                        Register& reg2 = findReg(ix2);

                        int v1 = reg1.getValue();
                        int v2 = reg2.getValue();
                        reg2.setReg( v2 - v1);
                        showInstructions(machine, "ADDR", -1, v2, v1, reg2.getValue(), "R" + std::to_string(ix2));
                        break;}
                    case Opcode::SVC: break;
                    case Opcode::TIXR: break;
                    
                    
                    //load commands
                    case Opcode::LDA: { 
                        int addr = getAddr(ni, opcode);
                        int val = getValueFromMemory(opcode, ni, addr);
                        int before = machine.A.getValue();
                        machine.A.setReg(val);
                        showInstructions(machine, "LDA", addr, val, before, machine.A.getValue(), "A");
                        break;}
                    case Opcode::LDX: { 
                        int addr = getAddr(ni, opcode);
                        int val = getValueFromMemory(opcode, ni, addr);
                        int before = machine.X.getValue();
                        machine.X.setReg(val);
                        showInstructions(machine, "LDX", addr, val, before, machine.X.getValue(), "X");
                        //machine.X.setReg(val);
                        break;}
                    case Opcode::LDB: {
                        int addr = getAddr(ni, opcode);
                        int val = getValueFromMemory(opcode, ni, addr);
                        int before = machine.B.getValue();
                        machine.B.setReg(val);
                        showInstructions(machine, "LDB", addr, val, before, machine.A.getValue(), "B");
                        //machine.B.setReg(val);
                        break;}
                    case Opcode::LDT: {
                        int addr = getAddr(ni, opcode);
                        int val = getValueFromMemory(opcode, ni, addr);
                        int before = machine.T.getValue();
                        machine.T.setReg(val);
                        showInstructions(machine, "LDT", addr, val, before, machine.A.getValue(), "T");
                        //machine.T.setReg(val);
                        break;}
                    case Opcode::LDS: {
                        int addr = getAddr(ni, opcode);
                        int val = getValueFromMemory(opcode, ni, addr);
                        int before = machine.S.getValue();
                        machine.S.setReg(val);
                        showInstructions(machine, "LDS", addr, val, before, machine.A.getValue(), "S");
                        //machine.S.setReg(val);
                        break;}
                    case Opcode::LDL: { 
                        int addr = getAddr(ni, opcode);
                        int val = getValueFromMemory(opcode, ni, addr);
                        int before = machine.L.getValue();
                        machine.L.setReg(val);
                        showInstructions(machine, "LDL", addr, val, before, machine.A.getValue(), "L");
                       // machine.L.setReg(val);
                        break;}
                    case Opcode::LDCH: {
                        int addr = getAddr(ni, opcode);
                        int val = getValueFromMemory(opcode, ni, addr);
                        int before = machine.A.getValue();
                        machine.A.setReg(val);
                        showInstructions(machine, "LDA", addr, val, before, machine.A.getValue(), "A");
                        //machine.A.setReg(val);
                        break;}


                    // store commands
                    case Opcode::STA: {
                        int val = machine.A.getValue();
                        int addr = getAddr(ni, opcode);
                        setValueOnMemory(opcode, ni, addr, val);
                        showStore(machine, "STA", addr, val);
                        break;}
                    case Opcode::STX: {
                        int val = machine.A.getValue();
                        int addr = getAddr(ni, opcode);
                        setValueOnMemory(opcode, ni, addr, val);
                        showStore(machine, "STX", addr, val);
                        break;}
                    case Opcode::STB: {
                        int val = machine.A.getValue();
                        int addr = getAddr(ni, opcode);
                        setValueOnMemory(opcode, ni, addr, val);
                        showStore(machine, "STB", addr, val);
                        break;}
                    case Opcode::STT: {
                        int val = machine.A.getValue();
                        int addr = getAddr(ni, opcode);
                        setValueOnMemory(opcode, ni, addr, val);
                        showStore(machine, "STT", addr, val);
                        break;}
                    case Opcode::STS: {
                        int val = machine.A.getValue();
                        int addr = getAddr(ni, opcode);
                        setValueOnMemory(opcode, ni, addr, val);
                        showStore(machine, "STS", addr, val);
                        break;}
                    case Opcode::STL: {
                        int val = machine.A.getValue();
                        int addr = getAddr(ni, opcode);
                        setValueOnMemory(opcode, ni, addr, val);
                        showStore(machine, "STL", addr, val);
                        break;}
                    case Opcode::STCH: {
                        int val = machine.A.getValue();
                        int addr = getAddr(ni, opcode);
                        setValueOnMemory(opcode, ni, addr, val);
                        showStore(machine,"STCH", addr, val);
                        break;}
                    case Opcode::STSW: {
                        int val = machine.SW.getValue();
                        int addr = getAddr(ni, opcode);
                        setValueOnMemory(opcode, ni, addr, val);
                        showStore(machine, "STSW", addr, val);
                        break;}


                    // jump commands
                    case Opcode::J: {
                        int addr = getAddr(ni, opcode);
                        //std::cout << "Zgoid se J\n";
                        //std::cout << "Izgled: " << (machine.PC.getValue() - 1) << " == " << addr << "\n";
                        if(beforePC == addr) {
                            ended = true;
                            std::cout << "pride do te instance\n";
                        }
                        machine.PC.setReg(addr);
                        break;}
                    case Opcode::JSUB: {
                        machine.L.setReg(machine.PC.getValue());
                        int addr = getAddr(ni, opcode);
                        machine.PC.setReg(addr);
                        break;}
                    case Opcode::RSUB: {
                        machine.PC.setReg(machine.L.getValue());
                        break;}
                    case Opcode::JEQ: {
                        if(machine.SW.getValue() == 0) {
                            int addr = getAddr(ni, opcode);
                            machine.PC.setReg(addr);
                        }
                        break;}
                    case Opcode::JGT: {
                        if(machine.SW.getValue() == 1) {
                            int addr = getAddr(ni, opcode);
                            machine.PC.setReg(addr);
                        }
                        break;}
                    case Opcode::JLT: {
                        if(machine.SW.getValue() == -1) {
                            int addr = getAddr(ni, opcode);
                            machine.PC.setReg(addr);
                        }
                        break;}

                    //Arithemtic commands
                    case Opcode::ADD: {
                        int addr = getAddr(ni, opcode);
                        int a = machine.A.getValue();
                        int val = getValueFromMemory(opcode, ni, addr);
                        machine.A.setReg(a + val);
                        showInstructions(machine, "ADD", addr, val, a, machine.A.getValue(), "A");
                        break;
                    }
                    case Opcode::DIV: {
                        int addr = getAddr(ni, opcode);
                        int a = machine.A.getValue();
                        int val = getValueFromMemory(opcode, ni, addr);
                        machine.A.setReg(a / val);
                        showInstructions(machine, "DIV", addr, val, a, machine.A.getValue(), "A");
                        break;
                    }
                    case Opcode::MUL: {
                        int addr = getAddr(ni, opcode);
                        int a = machine.A.getValue();
                        int val = getValueFromMemory(opcode, ni, addr);
                        machine.A.setReg(a * val);
                        showInstructions(machine, "MUL", addr, val, a, machine.A.getValue(), "A");
                        break;
                    }
                    case Opcode::AND: {
                        int addr = getAddr(ni, opcode);
                        int a = machine.A.getValue();
                        int val = getValueFromMemory(opcode, ni, addr);
                        machine.A.setReg(a & val);
                        showInstructions(machine, "AND", addr, val, a, machine.A.getValue(), "A");
                        break;
                    }
                    case Opcode::OR: {
                        int addr = getAddr(ni, opcode);
                        int a = machine.A.getValue();
                        int val = getValueFromMemory(opcode, ni, addr);
                        machine.A.setReg(a | val);
                        showInstructions(machine, "OR", addr, val, a, machine.A.getValue(), "A");
                        break;
                    }
                    case Opcode::SUB: {
                        int addr = getAddr(ni, opcode);
                        int a = machine.A.getValue();
                        int val = getValueFromMemory(opcode, ni, addr);
                        machine.A.setReg(a - val);
                        showInstructions(machine, "SUB", addr, val, a, machine.A.getValue(), "A");
                        break;
                    }
                    case Opcode::COMP: {
                        int addr = getAddr(ni, opcode);
                        int a = machine.A.getValue();
                        int val = getValueFromMemory(opcode, ni, addr);
                        if(a < val) {
                            machine.SW.setReg(-1);
                        } else if(a == val) {
                            machine.SW.setReg(0);
                        } else {
                            machine.SW.setReg(1);
                        }
                        break;
                    }

                }

                showRegisters(machine);
                showVariables(vars, machine.memory);


            //}
        }

        bool isEnded() {return ended;}

        void reset() {ended = false;}
};


class Reader {
    private:
        Machine& machine;
    
    public:

        Reader(Machine& mac) : machine(mac) {}
        int len = 0;
        int addr = 0;

        void loadIntoMemory(std::string& filename) {
            std::ifstream file(filename);

            if(!file.is_open()) {
                std::cerr << "File couldnt be open: " << filename << "\n";
                return;
            }

            std::string line;
            //int addr = 0;
            //int len = 0;

            while(std::getline(file, line)) {
                if(line.empty()) continue;

                char type = line[0];
                switch(type) {
                    case 'H': parseHearder(line); break;
                    case 'T': parseText(machine.memory, line); break;
                    case 'E': parseEnd(line); break;
                }
            }
        }



        void parseHearder(const std::string& line) {
            std::string name = line.substr(1,6);
            std::string codeAddress = line.substr(7, 6);
            std::string codeLen = line.substr(13,6);

            addr = std::stoi(codeAddress, nullptr, 16);
            len = std::stoi(codeLen, nullptr, 16);
        }

        void parseText(Memory& mem, const std::string& line) {
            std::string codeAddr = line.substr(1, 6);
            std::string codeLen = line.substr(7,2);
            addr = std::stoi(codeAddr, nullptr, 16);
            int length = std::stoi(codeLen, nullptr, 16);
           //std::cout << "dolžina tega T segmenta: " << length << "\n"; 
            std::string code = line.substr(9, length * 2);
            //std::cout << "koda, ko jo vpisujem: " << code << "\n";

            int j = 0;
            for(int i = 0; i < length; i++) {
                std::string b = code.substr(j,2);
                //std::cout << "The current Byte being loaded: " << b << "\n";
                int ib = std::stoi(b, nullptr, 16);
                uint8_t byte = static_cast<uint8_t>(ib);
                mem.setByte(addr, byte);
                addr++;
                j = j + 2;
            }
        }

        void parseEnd(const std::string& line) {
            int startAddr = std::stoi(line.substr(1, 6), nullptr, 16);
            
            machine.PC.setReg(startAddr);

        }


};



void disassasembly(std::string filename) {
    std::ifstream file(filename);

        if(!file.is_open()) {
            std::cerr << "File couldnt be open: " << filename << "\n";
            return;
        }

        std::string line;
            //int addr = 0;
            //int len = 0;

        while(std::getline(file, line)) {
            if(line.empty()) continue;

            std::cout << line << "\n";
        }
}



int main(int argc, char* argv[]) {

    std::atomic<bool> running{false};
    std::atomic<bool> stopRequested{false};
    std::atomic<bool> paused{false};
    std::string command;

    
    if(argc < 2) {
        std::cerr << "Missing a obj file and lst file\n";
        return 1;
    } else if( argc < 3) {
        std::cerr << "Missing lst file\n";
    }

    std::string file = argv[1];
    std::string vars = argv[2];

    //disassasembly(vars);
    
    VarVector vec;

    vec.setVariables(vars);

    Machine mach;
    Reader r = Reader(mach);

    //showVariables(vec.getVars(), mach.memory);

    r.loadIntoMemory(file);
    int addr = mach.PC.getValue();
    std::unique_ptr<Executor> e = std::make_unique<Executor>(mach, vec);

    std::cout << "SIC/XE Simulator REPL\n";
    std::cout << "Commands: start, stop, step, pause, showVars, showRegisters, quit\n";

    std::thread cpuThread([&] () {
        while(true) {
            if(stopRequested) break;
            if(running && !paused && !e->isEnded()) {
                e->execute();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    });

    while(!stopRequested && !e->isEnded()) {
        std::cout << "> ";
        if(!std::getline(std::cin, command)) break;

        if(command == "start") {
            running = true;
            paused = false;
            std::cout << "CPU started\n";
        } else if(command == "pause") {
            paused = true;
            std::cout << "CPU paused\n";
        } else if(command == "stop") {
            stopRequested = true;
            std::cout << "CPU stopped\n";
        } else if(command == "step") {
            
            e->execute();
            paused = true;
            //showVariables(vec.getVars(), mach.memory);
            //paused = true;
            //break;
        } else if(command == "quit") {
            stopRequested = true;
            std::cout << "Exiting REPL\n";
        } else if(command == "restart") {
            //vec.setVariables(); 
            running = false;
            paused = false;
            stopRequested = false;

            mach.reset();
            r.loadIntoMemory(file);
            e = std::make_unique<Executor>(mach, vec);

            std::cout << "Program restarted\n";
            //showVariables(vec.getVars(), mach.memory);
        } else if(command == "disasm") {
            disassasembly(vars);
        } else {
            std::cout << "Unknown command: " << command << "\n";
        }
        //showVariables(vec.getVars(), mach.memory);
        //std::cout << "Register A: " << mach.A.getValue() << "\n";

    }
    
    if(cpuThread.joinable()) {
        cpuThread.join();        
    }

    /*
    std::cout << "Dolzina kode: " << r.len << "\n";

    std::cout << "Memory content: \n";
    for(int i = 0; i < r.len; i++) {
        uint8_t b = mach.memory.getByte(addr);

        std::cout << std::hex << (int)b;
        addr++;
    }
    std::cout << "\n";*/



    return 0;

}