package gotoc

import (
	"encoding/json"
	"errors"
	"fmt"
)

type CalledHandler interface {
	OnCalled(method string, args []interface{}) (interface{}, error)
}

type Transport interface {
	Call(method string, args []interface{}, res interface{}) (interface{}, error)
	Listen(on CalledHandler)
}

type ctransport struct {
	socket   Socket
	onCalled CalledHandler

	debug bool
}

type rpcReq struct {
	Method string        `json:"method"`
	Params []interface{} `json:"params"`
}

type rpcRes struct {
	Error  interface{} `json:"error"`
	Result interface{} `json:"result"`
}

func NewTransportDefault(port int) Transport {
	s := GetSocket()
	return NewTransport(port, s)
}

func NewTransport(port int, s Socket) Transport {
	t := &ctransport{socket: s}
	t.socket.Listen(port, t)
	return t
}

type onCallReply struct {
	ret []byte
}

func (on *onCallReply) OnReply(r []byte) {
	on.ret = r
}

func (c *ctransport) Call(method string, args []interface{}, res interface{}) (interface{}, error) {

	req := rpcReq{Method: method, Params: args}
	in, err := json.Marshal(req)
	if err != nil {
		return nil, fmt.Errorf("gotoc: marshal err: %s", err)
	}

	//! NOTE Сейчас транспорт синхронный, поэтому всё просто
	onrep := &onCallReply{}
	c.socket.Send(Port_Bus, in, onrep)

	rep := rpcRes{}
	if res != nil {
		rep.Result = res
	}

	err = json.Unmarshal(onrep.ret, &rep)
	if err != nil {
		return nil, fmt.Errorf("gotoc: unmarshal err: %s", err)
	}

	if rep.Error != nil {
		return nil, errors.New(rep.Error.(string))
	}

	return rep.Result, nil
}

func (c *ctransport) Listen(h CalledHandler) {
	c.onCalled = h
}

func (c *ctransport) OnRequest(in []byte, out *[]byte) {

	fmt.Println("gotoc: request: ", string(in))

	if c.onCalled == nil {
		fmt.Println("gotoc: err: not set onCalled")
		return
	}

	req := rpcReq{}
	err := json.Unmarshal(in, &req)
	if err != nil {
		fmt.Println("gotoc: unmarshal err: ", err.Error())
		return
	}

	res := rpcRes{}
	res.Result, err = c.onCalled.OnCalled(req.Method, req.Params)

	if c.debug {
		fmt.Println("gotoc: called ", req.Method, " result: ", res.Result, ", err: ", err)
	}

	if err != nil {
		res.Error = err.Error()
	}

	*out, err = json.Marshal(res)
	if err != nil {
		fmt.Println("gotoc transport: marshal err: ", err)
	}
}
