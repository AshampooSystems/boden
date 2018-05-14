pipeline {
    agent none
    options {
        disableConcurrentBuilds()
    }
    stages {

        stage('Build webems') {
            agent {
                dockerfile {
                    filename 'Dockerfile_webems'
                    additionalBuildArgs  '-t boden_webems'
                    args '--volume ${WORKSPACE}:/boden -w /boden'
                }
            }

            steps {
                sh 'python build.py prepare --platform webems --build-system make'
                sh 'python build.py build --platform webems --config Release'
                stash includes: 'build/**/*', name: 'boden_webems_builddir'
            }
        }

        stage('Test webems') {
            agent {
                dockerfile {
                    filename 'Dockerfile_webems'
                    additionalBuildArgs  '-t boden_webems'
                    args '--volume ${WORKSPACE}:/boden -w /boden'
                    reuseNode true
                }
            }

            /* Note that we run the Release config. It is much faster and many browsers cannot even
               deal with the huge Javascript files of the debug build.*/
            steps {
            	unstash 'boden_webems_builddir'
                sh 'mkdir -p testresults'    
                
                /* The "|| true" at the end is there to ensure that the exit code of the command is ignored
                   (which will indicate an error if tests failed) and the script will continue.
                   Note that the junit command on the next line will fail if the test did not start or crashed, since then there
                   will be no results file.*/
                   
                /* XXX we have disabled the testboden junit output for the time being because of issue BDN-150.
                   Instead we run this test with console output for the time being.                   
                sh "xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py --platform webems --config Release --module testboden --stdout-file testresults/webems_testboden.xml -- run --reporter junit  || true"
                junit "testresults/webems_testboden.xml"*/

                sh "xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py --platform webems --config Release --module testboden -- run"

                sh "xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py --platform webems --config Release --module testbodenui --stdout-file testresults/webems_testbodenui.xml -- run --reporter junit  || true"
                junit "testresults/webems_testbodenui.xml"

                sh "xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py --platform webems --config Release --module testbodentiming --stdout-file testresults/webems_testbodentiming.xml -- run --reporter junit || true"
                junit "testresults/webems_testbodentiming.xml"
            }
        }

        stage('Build Android') {
            agent {
                dockerfile {
                    filename 'Dockerfile_android'
                    additionalBuildArgs  '-t boden_android'
                    args '--volume ${WORKSPACE}:/boden -w /boden'
                }
            }

            steps {
                sh 'python build.py prepare --platform android --build-system AndroidStudio'
                sh 'python build.py build --platform android --config Debug'
            }
        }

        

        /* We have to figure out how we can detect when the tests are done.
           Then we need to kill the emulator and end the "run" process.
           We cannot activate this before that is done.
        stage('Test Android') {
            agent {
                dockerfile {
                    filename 'Dockerfile_android'
                    additionalBuildArgs  '-t boden_android'
                    args '--volume ${WORKSPACE}:/boden -w /boden'
                    reuseNode true
                }
            }

            steps {
                sh 'python build.py --platform android --config Debug --module testboden run'
                sh 'python build.py --platform android --config Debug --module testbodenui run'
                sh 'python build.py --platform android --config Debug --module testbodentiming run'
            }
        }*/

        stage('Build Linux') {
            agent {
                dockerfile {
                    filename 'Dockerfile_linux'
                    additionalBuildArgs '-t boden_linux'
                    args '--volume ${WORKSPACE}:/boden -w /boden'
                }
            }
            steps {
                sh 'python build.py prepare --platform linux --build-system make'
                sh 'python build.py build --platform linux --config Release'
                
                stash includes: 'build/**/*', name: 'boden_linux_builddir'
            }
        }

        stage('Test Linux') {
            agent {
                dockerfile {
                    filename 'Dockerfile_linux'
                    additionalBuildArgs '-t boden_linux'
                    args '--volume ${WORKSPACE}:/boden -w /boden'
                }
            }
            steps {
                unstash 'boden_linux_builddir'
                sh 'python build.py --platform linux --config Release --module testboden run'
            }
        }

    }
}
