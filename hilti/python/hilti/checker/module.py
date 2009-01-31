# $Id$
#
# Modulel-level checks as well some applying to all instructions. 

from hilti.core import *
from checker import checker

@checker.pre(module.Module)
def _(self, m):
    if self._have_others:
        self.error(m, "*module* statement does not come first")
    
    if self._have_module:
        self.error(m, "more than one *module* statement")
        
    self._have_module = True
    self._module = m

@checker.post(module.Module)
def _(self, m):
    self._module = None
    
### Global ID definitions. 

@checker.when(id.ID, type.StructDeclType)
def _(self, id):
    self._have_others = True
    
    if self._in_function:
        self.error(id, "structs cannot be declared inside functions")
        
@checker.when(id.ID, type.StorageType)
def _(self, id):
    self._have_others = True

### Function definitions.

@checker.pre(function.Function)
def _(self, f):
    self._have_others = True
    self._in_function = True
    self._function = f
        
@checker.post(function.Function)
def _(self, f):
    self._in_function = False
    self._function = None
    
    if not self._have_module:
        self.error(f, "input file must start with module declaration")
        
### Instructions.
@checker.when(instruction.Instruction)
def _(self, i):
    self._have_others = True
    
    # Check that signature maches the actual operands. 
    def typeError(actual, expected, tag):
        self.error(i, "type of %s does not match signature (expected %s but is %s) " % (tag, str(expected), str(actual)))
        
    def checkOp(op, sig, tag):
        if sig and not op and not type.isOptional(sig):
            self.error(i, "%s missing" % tag)
            return
            
        if op and not sig:
            self.error(i, "superfluous %s" % tag)
            return
            
        if op and sig and not op.type() == sig:
            typeError(type.name(op.type()), type.name(sig), tag)
            return
        
    checkOp(i.op1(), i.signature().op1(), "operand 1")
    checkOp(i.op2(), i.signature().op2(), "operand 2")
    checkOp(i.op3(), i.signature().op3(), "operand 3")
    checkOp(i.target(), i.signature().target(),"target")

    if i.target() and not isinstance(i.target(), instruction.IDOperand):
        self.error(i, "target must be an identifier")