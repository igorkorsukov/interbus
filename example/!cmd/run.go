package main

import (
	"bufio"
	"fmt"
	"log"
	"os/exec"
)

func main() {

	{
		fmt.Println("start build")
		out, err := exec.Command("bash.exe", "build_win.sh").Output()
		if err != nil {
			log.Fatal(err)
		}

		fmt.Println(string(out))
	}

	fmt.Println("end build")
	fmt.Println("run")

	cmd := exec.Command("build/bin/minimal.exe")
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		log.Fatal(err)
	}

	scanner := bufio.NewScanner(stdout)
	go func() {
		for scanner.Scan() {
			fmt.Println(scanner.Text())
		}
	}()

	if err := cmd.Start(); err != nil {
		log.Fatal(err)
	}

	if err := cmd.Wait(); err != nil {
		log.Fatal(err)
	}

}
