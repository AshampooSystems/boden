pipeline {
    agent none
    stages {
        stage('Build Linux') {
        	agent {
        		label 'master'
        	}
            steps {
                sh '/data/boden-ci/boden-gtk-ubuntu/docker-build.sh "${WORKSPACE}"'
            }
        }
    }
}