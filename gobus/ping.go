package gotoc

import "fmt"

type ping struct {
}

func (p *ping) OnRequest(in []byte, out *[]byte) {

	t := GetSocket()
	if t == nil {
		fmt.Println("gotoc: socket not inited")
	}

	fmt.Println("gotoc: onPing in: ", string(in))

	s := string(in)
	s += " -> ping from go"

	onrep := &onPingReply{}
	t.Send(Port_Ping, []byte(s), onrep)

	fmt.Println("gotoc: onPing reply: ", string(onrep.ret))

	s2 := string(onrep.ret)
	s2 += " -> pong from go"

	fmt.Println("gotoc: onPing ret: ", s2)

	*out = []byte(s2)
}

type onPingReply struct {
	ret []byte
}

func (on *onPingReply) OnReply(r []byte) {
	on.ret = r
}
