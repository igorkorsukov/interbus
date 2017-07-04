package gotoc

func ArgInt(index int, args []interface{}) int {
	if len(args) > index {
		v, ok := args[index].(float64)
		if ok {
			return int(v)
		}
	}
	return 0
}

func ArgInt64(index int, args []interface{}) int64 {
	if len(args) > index {
		v, ok := args[index].(float64)
		if ok {
			return int64(v)
		}
	}
	return 0
}

func ArgString(index int, args []interface{}) string {
	if len(args) > index {
		v, ok := args[index].(string)
		if ok {
			return v
		}
	}
	return ""
}
