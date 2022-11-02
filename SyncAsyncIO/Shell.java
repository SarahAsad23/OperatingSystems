/**
 * Sarah Asad
 * CSS 430 - Program 1J
 * Shell,java
 *
 * this program functions like a Linux command interpreter. and has 3 main functions:
 * 1) display a prompt to the console – indicating that the shell is ready to accept a command from the user.
 * 2) read the line of input from the console.
 * 3) and spawn and have a new process execute the user’s command.
 */
import java.io.*;
import java.util.*;
public class Shell extends Thread {
    public Shell()  {
    }

    public void run(){
        for(int line =1; ; line++){
            String cmdLine = "";
            do{
                //buffer used to read and write to the console
                StringBuffer inputBuf = new StringBuffer();
                //printing out the prompt in the form: shell[1]% to the console
                SysLib.cerr("shell[" + line + "]% ");
                //reading the input
                SysLib.cin(inputBuf);
                //converting the input to a string
                cmdLine = inputBuf.toString();
            }while(cmdLine.length() == 0);
            //each space-delimited word is stored into a different array element
            String[] args = SysLib.stringToArgs(cmdLine);
            int first = 0;
            for(int i = 0; i < args.length; i++){
                if(args[i].equals(";") || args[i].equals("&") || i == args.length - 1){
                    String[] command = generateCmd(args, first, (i==args.length - 1) ? i + 1 : i);
                    //bool used to see if exec was successfully able to execute
                    boolean success = true;
                    if(command != null){
                        //check if command[0] is "exit" then we call SysLib.exit and return to break out of program
                        if(command[0].equals("exit")){
                            SysLib.exit();
                            return;
                        }
                        //otherwise pass the command to SysLib.exec()
                        else{
                            //if error in exec, success = false, otherwise success = true.
                            success = SysLib.exec(command) == -1 ? success = false : success == true;
                        }
                        //do nothing if args[i] equals "&"
                        //keep calling SysLib.join() if args[i] equals ";" and if success = true
                        if((!args[i].equals("&")) && (success == true) ) {
                            SysLib.join();
                        }
                    }
                    first = i + 1; //go to the next command delimited by ";" or "&"
                }
            }
        }
    }

    /*
     * Parameters: a string of space delimited words (String[] arr) as well as a starting index (int startIndex), and ending index (int endIndex)
     * Return Value: String[]
     * This method iterates through the passed in arr array beginning at the starting index and ending at the ending index and copies the string
     * values into the local string array which is then returned.
     */
    String[] generateCmd(String[] arr, int startIndex, int endindex){

        //error checking to make sure a command actually exists - size is not less than or equal to 0
        if(endindex - startIndex <= 0){
            return null;
        }

        //create a new array which is allocated to the size of the command
        String[] command = new String[endindex - startIndex];
        //looping through from the starting index to the ending index
        for(int i = startIndex; i < endindex; i++){
            //copying over the elements from arr to command
            command[i - startIndex] = arr[i];
        }
        return command;
    }

    public static void main(String[] args) {
        // TODO Auto-generated method stub
        Shell shell = new Shell();
        shell.run();
    }
}