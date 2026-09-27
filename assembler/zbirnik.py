import sys;

class instructionF1:
    def __init__(self, name=None):
        self.name = name

class instructionF2:
    def __init__(self, name=None, reg1=None, reg2=None):
        self.name = name
        self.reg1 = reg1
        self.reg2 = reg2

class instructionF3:
    def __init__(self, name=None, address=None):
        self.name = name
        self.address = address

class instructionF4:
    def __init__(self, name=None, address=None):
        self.name = name
        self.address = address

class Symbol:
    def __init__(self, name, address, length=0, defined=False):
        self.name = name
        self.address = address
        self.length = length
        self.defined = defined

class SymbolTable:
    def __init__(self):
        self.symbols = {}

    def add_symbol(self, name, address, length=0, defined=False):
        if name in self.symbols:
            raise ValueError(f"Symbol {name} already exists.")
        self.symbols[name] = Symbol(name, address, length, defined)

    def get_symbol(self, name):
        return self.symbols.get(name)

    def find_symbol(self, name):
        return name in self.symbols

MNEMONICS = {
    "ADD", "ADDF", "ADDR", "AND",
    "CLEAR",
    "COMP", "COMPF", "COMPR",
    "DIV", "DIVF", "DIVR",
    "FIX", "FLOAT",
    "HIO",
    "J", "JEQ", "JGT", "JLT", "JSUB",
    "LDA", "LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX", "LPS",
    "MUL", "MULF", "MULR",
    "NORM",
    "OR",
    "RD", "RMO", "RSUB",
    "SHIFTL", "SHIFTR",
    "SIO", "SSK",
    "STA", "STB", "STCH", "STF", "STI", "STL", "STS", "STSW", "STT", "STX",
    "SUB", "SUBF", "SUBR",
    "SVC",
    "TD", "TIO", "TIX", "TIXR",
    "WD"
}

DIRECTIVES = {
    "START","END","WORD","BYTE","RESW","RESB","BASE","NOBASE"
}


def f3_f4(instr):
    if instr[0] == '+':
        match instr[1:]:
            case "ADD" | "ADDF" | "AND" | "COMP" | "COMPF" | "DIV" | "DIVF" | "J" | "JEQ" | "JGT" | "JLT" | "JSUB" | "LDA" | "LDB" | "LDCH" | "LDF" | "LDL" | "LDS" | "LDT" | "LDX" | "LPS" | "MUL" | "MULF" | "OR" | "RD" | "RSUB" | "SSK" | "STA" | "STB" | "STCH" | "STF" | "STI" | "STL" | "STS" | "STSW" | "STT" | "STX" | "SUB" | "SUBF" | "TD" | "TIX" | "WD":  return 4  # Default to format 3 for other instructions
            case _: return 0
    else:
        match instr:
            case "ADD" | "ADDF" | "AND" | "COMP" | "COMPF" | "DIV" | "DIVF" | "J" | "JEQ" | "JGT" | "JLT" | "JSUB" | "LDA" | "LDB" | "LDCH" | "LDF" | "LDL" | "LDS" | "LDT" | "LDX" | "LPS" | "MUL" | "MULF" | "OR" | "RD" | "RSUB" | "SSK" | "STA" | "STB" | "STCH" | "STF" | "STI" | "STL" | "STS" | "STSW" | "STT" | "STX" | "SUB" | "SUBF" | "TD" | "TIX" | "WD":  return 3  # Default to format 3 for other instructions
            case _: return 0

def instruction_type(instr):
    match instr:
        case "FIX" | "FLOAT" | "HIO" | "NORM" | "SIO" | "TIO": return 1
        case "ADDR" | "CLEAR" | "COMPR" | "DIVR" | "MULR" | "RMO" | "SHIFTL" | "SHIFTR" | "SUBR" | "SVC" | "TIXR": return 2
        case _: return f3_f4(instr)

def reg_index(reg):
    match reg:
        case "A": return 0
        case "X": return 1
        case "L": return 2
        case "B": return 3
        case "S": return 4
        case "T": return 5
        case "F": return 6
        case _: return int(reg)

def ukaz_arg0(instr):
    match instr:
        case "FIX": return True
        case "FLOAT": return True
        case "HIO": return True
        case "NORM": return True
        case "RSUB": return True
        case "SIO": return True
        case "STI": return True
        case "TIO": return True
        case _: return False

def ukaz_arg1(instr):
    match instr:
        case "ADD" : return True
        case "ADDF": return True
        case "AND": return True
        case "CLEAR": return True
        case "COMP": return True
        case "COMPF": return True
        case "DIV": return True
        case "DIVF": return True
        case "J": return True
        case "JEQ": return True
        case "JGT": return True
        case "JLT": return True
        case "JSUB": return True
        case "LDA": return True
        case "LDB": return True
        case "LDCH": return True
        case "LDF": return True
        case "LDL": return True
        case "LDS": return True
        case "LDT": return True
        case "LDX": return True
        case "LPS": return True
        case "MUL": return True
        case "MULF": return True
        case "OR": return True
        case "RD": return True
        case "SSK": return True
        case "STA": return True
        case "STB": return True
        case "STCH": return True
        case "STF": return True
        case "STI": return True
        case "STL": return True
        case "STS": return True
        case "STSW": return True
        case "STT": return True
        case "STX": return True
        case "SUB": return True
        case "SUBF": return True
        case "SVC": return True
        case "TD": return True
        case "TIX": return True
        case "TIXR": return True
        case "WD": return True
        case _: return False

def ukaz_arg2(instr):
    match instr:
        case "ADDR": return True
        case "COMPR": return True
        case "DIVR": return True
        case "MULR": return True
        case "RMO": return True
        case "SHIFTL": return True
        case "SHIFTR": return True
        case "SUBR": return True
        case _: return False



def opcode(instr):
    match instr:
        case "ADD":  return 0x18
        case "ADDF": return 0x58
        case "ADDR":  return 0x90
        case "AND": return 0x40
        case "CLEAR": return 0xB4
        case "COMP": return 0x28
        case "COMPF": return 0x88
        case "COMPR": return 0xA0
        case "DIV": return 0x24
        case "DIVF": return 0x64
        case "DIVR": return 0x9C
        case "FIX": return 0xC4
        case "FLOAT": return 0xC0
        case "HIO": return 0xF4
        case "J": return 0x3C
        case "JEQ": return 0x30
        case "JGT": return 0x34
        case "JLT": return 0x38
        case "JSUB": return 0x48
        case "LDA": return 0x00
        case "LDB": return 0x68
        case "LDCH": return 0x50
        case "LDF": return 0x70
        case "LDL": return 0x08
        case "LDS": return 0x6C
        case "LDT": return 0x74
        case "LDX": return 0x04
        case "LPS": return 0xD0
        case "MUL": return 0x20
        case "MULF": return 0x60
        case "MULR": return 0x98
        case "NORM": return 0xC8
        case "OR": return 0x44
        case "RD": return 0xD8
        case "RMO": return 0xAC
        case "RSUB": return 0x4C
        case "SHIFTL": return 0xA4
        case "SHIFTR": return 0xA8
        case "SIO": return 0xF0
        case "SSK": return 0xEC
        case "STA": return 0x0C
        case "STB": return 0x78
        case "STCH": return 0x54
        case "STF": return 0x80
        case "STI": return 0xD4
        case "STL": return 0x14
        case "STS": return 0x7C
        case "STSW": return 0xE8
        case "STT": return 0x84
        case "STX": return 0x10
        case "SUB": return 0x1C
        case "SUBF": return 0x5C
        case "SUBR": return 0x94
        case "SVC": return 0xB0
        case "TD": return 0xE0
        case "TIO": return 0xF8
        case "TIX": return 0x2C
        case "TIXR": return 0xB8
        case "WD": return 0xDC
        case _: raise ValueError("Invalid instruction")

def tokenizer(line, comment):
    command = ""
    for i in range(len(line)):
        #print("Char: ", line[i])
        if line[i] == '.' or line[i] == '\n':
            comment = True
            #print("Se zgodi\n")
            continue
        elif comment == False:
            command += line[i]

    return command

def addressing_type(argument):
    if argument.startswith("#"):
        return "immediate"
    elif argument.startswith("@"):
        return "indirect"
    else:
        return "simple"

def editor(split_command):
    
    if ukaz_arg0(split_command[0]) or (len(split_command) > 1 and ukaz_arg0(split_command[1])):
        if len(split_command) > 2:
            print("Napaka: Preveč argumentov za ukaz brez argumentov")
    elif ukaz_arg1(split_command[0]) or (len(split_command) > 1 and ukaz_arg1(split_command[1])):
        if len(split_command) > 3 :
            print("Napaka: Preveč argumentov za ukaz z enim argumentom")
    elif ukaz_arg2(split_command[0]):
        print("Ukaz z dvema argumentoma brez deklaracije")
        if len(split_command) < 3:
            
            sc = split_command[1].split(",")
            del split_command[1]
            split_command.append(sc[0])
            split_command.append(sc[1])
        elif len(split_command) == 4:
            del split_command[2]
        elif len(split_command) == 3 :
            sc = split_command[1]
            split_command.pop(1)
            split_command.insert(1, sc.strip(","))
            sc = split_command[2]
            split_command.pop(2)
            split_command.insert(2, sc.strip(","))



    elif len(split_command) > 1 and ukaz_arg2(split_command[1]):    
        print("Ukaz z dvema argumentoma z deklaracijo")
        if len(split_command) < 4:
            sc = split_command[2].split(",")
            del split_command[2]
            split_command.append(sc[0])
            split_command.append(sc[1])
        elif len(split_command) == 5:
            del split_command[3]
            
    return split_command



def cleaner(filename):
    clean_code = []

    with open(filename, "r") as f:
        line = f.readline()
        while line:
            command = ""
            #print("Readline: ", line)
            #print("Stripped line: ", line.strip())
            if line.strip() == "":
                line = f.readline()
                continue

            else:    
                comment = False
            
                command = tokenizer(line, comment)

                if command.strip() != "":
                    print("Command: ", command)
                    split_command = command.split()
                    print("Split command: ", split_command)
                    edited_command = editor(split_command)
                    clean_code.append(edited_command)

            line = f.readline()

    return clean_code

def fill_symtab(code, symtab, prog_len):
    lc = 0
    start = 0
    #for i in range(len(code)):
     #   print("Code line: ", code[i][0])

    for i in range(len(code)):
        print("\nProcessing line ", i, ": ", code[i])

        if i == 0 and code[i][1] == "START":
            start = int(code[i][-1])
            lc = start
            symtab.add_symbol(code[i][0], lc, 0, True)
            continue

        label = code [i][0]

        if len(code[i]) > 1:
            mnemonic = code[i][1]



        if mnemonic == "WORD":
            if label:
                symtab.add_symbol(label, lc, 3, True)
            print("LC before WORD line ", i, ": ", hex(lc))
            lc += 3
            print("LC after WORD line ", i, ": ", hex(lc))
            continue


        if mnemonic == "BYTE":
            size = 1 if code[i][2].startswith("C'") else (len(code[i][2]) - 3) // 2
            if label:
                symtab.add_symbol(label, lc, size, True)
            print("LC before BYTE line ", i, ": ", hex(lc))
            lc += size
            print("LC after BYTE line ", i, ": ", hex(lc))
            continue

        if mnemonic == "RESW":
            size = int(code[i][2]) * 3
            if label:
                symtab.add_symbol(label, lc, size, True)
            print("LC before RESW line ", i, ": ", hex(lc))
            lc += size
            print("LC after RESW line ", i, ": ", hex(lc))
            continue

        if mnemonic == "RESB":
            size = int(code[i][2])
            if label:
                symtab.add_symbol(label, lc, size, True)
            print("LC before RESB line ", i, ": ", hex(lc))
            lc += size
            print("LC after RESB line ", i, ": ", hex(lc))
            continue

        if label:
            symtab.add_symbol(label, lc, instruction_type(mnemonic), True)

        print("LC before line ", i, ": ", hex(lc))

        lc += instruction_type(mnemonic)

        print("LC after line ", i, ": ", hex(lc))


    prog_len = lc - start
    return prog_len
    #print("\nFinal LC: ", lc -start)
    #return symtab
    
def normailze(code):
    for line in code:
        if ukaz_arg0(line[0]) or ukaz_arg1(line[0]) or ukaz_arg2(line[0]):
            line.insert(0, "")
        elif line[0].startswith('+') and (ukaz_arg0(line[0][1:]) or ukaz_arg1(line[0][1:]) or ukaz_arg2(line[0][1:])):
            line.insert(0, "")
        elif line[0] in {"BASE", "NOBASE", "RESB", "RESW", "END"} :
            line.insert(0,"")
    
    return code

def encode_WORD(operand):
    value = int(operand)

    if value < 0:
        value &= 0xFFFFFF

    return [(value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF]

def encode_BYTE(operand):
    bytes_list = []

    if operand.startswith("C'") and operand.endswith("'"):
        content = operand[2:-1]
        for char in content:
            bytes_list.append(ord(char))
    elif operand.startswith("X'") and operand.endswith("'"):
        content = operand[2:-1]
        for i in range(0, len(content), 2):
            byte = int(content[i:i+2], 16)
            bytes_list.append(byte)
    else:
        raise ValueError("Invalid BYTE operand format")

    return bytes_list
    

def encode(line, symtab, lc):
    mnemonic = line[1]
    instr_type = instruction_type(mnemonic)

    if instr_type == 1:
        return opcode(mnemonic)

    elif instr_type == 2:
        reg1 = reg_index(line[2])
        reg2 = 0
        if len(line) > 3:
            reg2 = reg_index(line[3])
        return (opcode(mnemonic) << 8) | (reg1 << 4) | reg2

    elif instr_type == 3:
        op = opcode(mnemonic)

        n = 1
        i = 1

        if len(line) > 2:
            
            if addressing_type(line[2]) == "immediate":
                n = 0
                i = 1
            elif addressing_type(line[2]) == "indirect":
                n = 1
                i = 0
            else:
                n = 1
                i = 1
        
        op |= (n << 1) | i 

        x = 1

        if line[-1] == "X":
            x = 1
        else:
            x = 0

        e = 0

        disp = 0
        if len(line) > 2 and addressing_type(line[2]) != "immediate":
            address = 0
            if symtab.find_symbol(line[2]):
                address = symtab.get_symbol(line[2]).address
            else: 
                address = line[2]

            if addressing_type(line[2]) == "indirect":
                a = address[1:]
                if symtab.find_symbol(a):
                    address = symtab.get_symbol(a).address
                else:
                    address = a

            disp = int(address) - (lc + 3)

            p = 0
            b = 0
            if -2048 <= disp <= 2047:
                p = 1
                b = 0
        
            elif symtab.find_symbol("LABEL"):
                base_register = symtab.get_symbol("LABEL").address
                disp = address - base_register
                p = 0
                b = 1
        else:

            p = 0
            b = 0
            if len(line) > 2:
                disp = int(line[2][1:])

        return (op << 16) | (x << 15) | (b << 14) | (p << 13) | (e << 12) | (disp & 0xFFF)

    elif instr_type == 4:

        op = opcode(mnemonic[1:])

        n = 1
        i = 1

        if len(line) > 2:
            
            if addressing_type(line[2]) == "immediate":
                n = 0
                i = 1
            elif addressing_type(line[2]) == "indirect":
                n = 1
                i = 0
            else:
                n = 1
                i = 1
        
        op |= (n << 1) | i 

        x = 1

        if line[-1] == "X":
            x = 1
        else:
            x = 0

        e = 1

        address = line[2]

        if symtab.find_symbol(line[2]):
            address = symtab.get_symbol(line[2]).address
        
        disp = address

        p = 0
        b = 0

        return (op << 24) | (x << 23) | (b << 22) | (p << 21) | (e << 20) | (disp & 0xFFFFF)

    else:
        if line[1] == "WORD":
            bytes_list = encode_WORD(line[2])
            encoded = 0
            for byte in bytes_list:
                encoded = (encoded << 8) | byte
            return encoded
        elif line[1] == "BYTE":
            bytes_list = encode_BYTE(line[2])
            encoded = 0
            for byte in bytes_list:
                encoded = (encoded << 8) | byte
            return encoded
        else:
            return 0

def int_to_bytes(value, length):
    return [(value >> (8 * (length - 1 - i))) & 0xFF for i in range(length)]

def byte_to_hex(byte_list):
    return "".join(f"{b:02X}" for b in byte_list)


def main():

    filename = sys.argv[1]

    clean_code = cleaner(filename)
    
    print("Only code: ")

    

    for i in range(len(clean_code)):
        print("\n", clean_code[i])

    symtab = SymbolTable()
    cc = normailze(clean_code)
    pl = 0
        
    prog_len = fill_symtab(cc, symtab, pl)
    print("\nSymbol Table:")
    for symbol in symtab.symbols.values():
        print(f"Name: {symbol.name}, Address: {hex(symbol.address)}, Length: {symbol.length}, Defined: {symbol.defined}")    

    prog_name = cc[0][0]
    start = int(cc[0][-1])
    prog_start = start
    T_records = []
    M_records = []
    lst_lines = []

    current_T = {"start": None, "bytes":[]}

    def emit_bytes(addr, byte_list):

        if not current_T["bytes"]:
            current_T["start"] = addr
        

        if len(current_T["bytes"]) + len(byte_list) > 30:
            flush_t()
            current_T["start"] = addr

        current_T["bytes"].extend(byte_list)

    def flush_t():
        if current_T["bytes"]:
            T_records.append((current_T["start"], current_T["bytes"][:]))
            current_T["start"] = None
            current_T["bytes"].clear()
    
    print(f"\nProgram Name: {prog_name}, Start Address: {hex(start)}, Length: {hex(prog_len)}")
    
    #cc = normailze(clean_code)

    lc = int(cc[0][-1])
    prvi = True

    for line in cc:
        if len(line) > 1:
            mnemonic = line[1]
        else:
            mnemonic = line[0]
        address = lc

        if mnemonic== "WORD":
            encoded = encode(line, symtab, lc)
            bites = int_to_bytes(encoded, 3)
            emit_bytes(lc, bites)
            print(f"\nEncoded:", hex(encoded))
            lc += 3
            addr_str = f"{address:04X}"
            obj_str = byte_to_hex(bites).ljust(8)

            lst_lines.append(f"{addr_str}  {obj_str:<8} {line[0]:<8} {mnemonic:<7} {' '.join(line[2:])}")
            continue
        
        if mnemonic == "BYTE":
            encoded = encode(line, symtab, lc)
            print(f"\nEncoded:", hex(encoded))
            size = len(line[2]) - 3 if line[2].startswith("C'") else (len(line[2]) - 3) // 2
            bites = int_to_bytes(encoded, size)
            emit_bytes(lc, bites)
            lc += size
            addr_str = f"{address:04X}"
            obj_str = byte_to_hex(bites).ljust(8)

            lst_lines.append(f"{addr_str}  {obj_str:<8} {line[0]:<8} {mnemonic:<7} {' '.join(line[2:])}")
            continue

        if mnemonic == "RESW":
            flush_t()
            lc += int(line[2]) * 3
            continue

        if mnemonic == "RESB":
            flush_t()
            lc += int(line[2])
            continue

        if mnemonic in {"START", "END", "BASE", "NOBASE"}:
            flush_t()
            continue

        if mnemonic not in {"START", "END", "BASE", "NOBASE", "WORD", "BYTE", "RESW", "RESB"} and prvi:
            prvi = False
            prog_start = lc

        encoded = encode(line, symtab, lc)
        print(f"\nEncoded:", hex(encoded))

        instr_type = instruction_type(mnemonic)
        bites = int_to_bytes(encoded, instr_type)
        emit_bytes(lc, bites)

        if instr_type == 4 and symtab.find_symbol(line[2]):
            M_records.append({"addr": lc + 1, "len": 5, "symbol": line[2]})

        lc += instr_type

        addr_str = f"{address:04X}"
        obj_str = byte_to_hex(bites).ljust(8)

        lst_lines.append(f"{addr_str}  {obj_str:<8} {line[0]:<8} {mnemonic:<7} {' '.join(line[2:])}")

    flush_t()
    obj_code = []

    #print("\nText Records:")

    obj_code.append(f"H{prog_name:<6}{start:06X}{prog_len:06X}")
    #print(f"H{prog_name:<6}{start:06X}{prog_len:06X}")

    for start_addr, byte_list in T_records:
        lenght = len(byte_list)
        data = "".join(f"{b:02X}" for b in byte_list)
        obj_code.append(f"T{start_addr:06X}{lenght:02X}{data}")
        print(f"T{start_addr:06X}{lenght:02X}{data}")

    for m in M_records:
        obj_code.append(f"M{m['addr']:06X}{m['len']:02X}+{m['symbol']}") 
        print(f"M{m['addr']:06X}{m['len']:02X}+{m['symbol']}") 

    obj_code.append(f"E{prog_start:06X}")
    print(f"E{prog_start:06X}")


    with open("test.obj", "w") as obj_file:
        for record in obj_code:
            obj_file.write(record + "\n")
    #for i in range(len(cc)):
     #   print("\nNormalized code: ", cc[i])

    with open("test.lst", "w") as lst_file:
        for line in lst_lines:
            lst_file.write(line + "\n")




if __name__ == "__main__":
    main()