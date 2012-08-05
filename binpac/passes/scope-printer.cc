
#include "expression.h"
#include "statement.h"
#include "scope.h"

#include "scope-printer.h"
#include "printer.h"

using namespace binpac;
using namespace binpac::passes;

ScopePrinter::ScopePrinter(std::ostream& out) : Pass<AstInfo>("ScopePrinter"), _out(out)
{
}

ScopePrinter::~ScopePrinter()
{
}

bool ScopePrinter::run(shared_ptr<ast::NodeBase> module)
{
    return processAllPreOrder(module);
}

void ScopePrinter::visit(statement::Block* b)
{
    _out << "<<< Scope for " << b->render() << std::endl;
    b->scope()->dump(_out);
    _out << ">>>" << std::endl;
}
