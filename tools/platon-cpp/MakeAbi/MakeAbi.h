
#include "llvm/IR/Type.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/ADT/SmallString.h"
#include <map>
#include <vector>

enum AttrKind {ActionKind, EventKind, OtherKind};
typedef std::pair<AttrKind, unsigned> Attr;

class MakeAbi {
  public:
    llvm::json::Value contents = llvm::json::Array{};
    std::map<llvm::DIType*, llvm::StringRef> TypeMap;
    std::string StringBuf;

    llvm::json::Value handleAction(llvm::DISubprogram* SP, llvm::json::Value Params, bool isConst);
    llvm::json::Value handleEvent(llvm::DISubprogram* SP, llvm::json::Value Params, unsigned num);
    void handleSubprogram(llvm::DISubprogram* SP, std::vector<llvm::DILocalVariable*> &LVs, Attr attr);

    llvm::StringRef handleVector(llvm::DINode* Node, llvm::DICompositeType* CT);
    llvm::StringRef handleStructType(llvm::DINode* Node, llvm::DICompositeType* CT);
    llvm::StringRef handleCompositeType(llvm::DINode* Node, llvm::DICompositeType* CT);

    llvm::StringRef handleDerivedType(llvm::DINode* Node, llvm::DIDerivedType* DevT);
    llvm::StringRef handleBasicType(llvm::DINode* Node, llvm::DIBasicType* BT);
    llvm::json::Value handleElem(llvm::DINode* Node, llvm::DIType* DT);

    llvm::StringRef handleType_(llvm::DINode* Node, llvm::DIType* DT);
    llvm::StringRef handleType(llvm::DINode* Node, llvm::DIType* DT);
};