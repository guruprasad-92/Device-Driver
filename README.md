Device-Driver
------ GIT related info ----------
update the local project from github :
    In the working directory run the following ommand    
    $ git pull origin master

Update the github project from local project :
    In working directory, run the following commands.
    $ git add .
    $ git commit -m "commit message"
    $ git push origin master

Remove a file from git not from the disk.
	$git rm --cached file_name
	$git commit --amend -CHEAD
	$git push
