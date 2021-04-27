package parse


var cppTemplate = `
{{- range .Includes}}
{{- .}}
{{end}}
namespace {{.Namespace}} {
{{- range $, $value := .Structs}}
struct {{$value.Name -}};
{{- end}}
{{range $key, $value := .Structs}}
struct {{$value.Name -}} {
{{- range $value.Fields}}
	{{.Type}} {{.Name -}};
{{- end}}
	SOLIDITY_SERIALIZE({{generateSerialize $value}})
};
{{end}}


{{- range .Functions }}
{{- if eq .Output "void"}}
template <typename value_type, typename gas_type>
			bool {{.Name}}(const Address &_addr_,
			const value_type &_value_,
			const gas_type &gas {{generateFuncInputs .Inputs}}){
			return sol_call(_addr_, _value_, gas {{generateInputs .Inputs}});
			}
{{- else}}
template <typename value_type, typename gas_type>
			std::tuple<{{.Output}},bool> {{.Name}}(const Address &_addr_,
			const value_type &_value_,
			const gas_type &gas {{generateFuncInputs .Inputs}}){
			return sol_call_with_return_value<{{.Output}}>(_addr_, _value_, gas {{generateInputs .Inputs}});
			}
{{- end}}
{{- end}}
}
`
