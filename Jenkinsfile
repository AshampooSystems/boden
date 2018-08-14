pipeline {
    agent none
    options {
        disableConcurrentBuilds()
    }
    stages {
        stage('Build Linux') {
            agent {
                dockerfile {
                    filename 'Dockerfile_linux'
                    additionalBuildArgs '-t boden_linux'
                    args '--volume ${WORKSPACE}:/boden -w /boden'
                }
            }
            steps {
                sh 'python build.py prepare --platform linux --build-system make --config Release'
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

                sh 'mkdir -p testresults'

                sh 'python build.py --platform linux --config Release --module testboden -- run --out testresults/linux_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                junit "testresults/linux_testboden.xml"

                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py --platform linux --config Release --module testbodenui -- run --out testresults/linux_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                junit "testresults/linux_testbodenui.xml"

                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py --platform linux --config Release --module testbodentiming -- run --out testresults/linux_testbodentiming.xml --reporter junit --reporter console --print-level 2 || true'
                junit "testresults/linux_testbodentiming.xml"
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
                sh 'python build.py prepare --platform android --arch x86_64 --build-system AndroidStudio'
                sh 'python build.py build --platform android --arch x86_64 --build-system AndroidStudio --config Debug'
                stash includes: 'build/**/*', name: 'boden_android_builddir'
            }
        }

        
        /* Disabled tests because we were unable to get the android emulator to run in AWS.
           Neither --device /dev/kvm nor --privileged seems to work.
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
                unstash 'boden_android_builddir'

                sh 'mkdir -p testresults'   

                sh 'python build.py --platform android --arch x86_64 --build-system AndroidStudio --config Debug --module testboden --run-output-file testresults/android_testboden.xml --run-android-fetch-output-from {DATA_DIR}/testresults.xml -- run --out {DATA_DIR}/testresults.xml --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
                junit "testresults/android_testboden.xml"

                sh 'python build.py --platform android --arch x86_64 --build-system AndroidStudio --config Debug --module testbodenui --run-output-file testresults/android_testbodenui.xml --run-android-fetch-output-from {DATA_DIR}/testresults.xml -- run --out {DATA_DIR}/testresults.xml --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
                junit "testresults/android_testbodenui.xml"

                sh 'python build.py --platform android --arch x86_64 --build-system AndroidStudio --config Debug --module testbodentiming --run-output-file testresults/android_testbodentiming.xml --run-android-fetch-output-from {DATA_DIR}/testresults.xml -- run --out {DATA_DIR}/testresults.xml --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
                junit "testresults/android_testbodentiming.xml"
            }
        }*/


        stage('Build macOS') {
            agent { label 'macOS' }
            steps {
                sh 'python build.py prepare --platform mac --build-system Xcode'
                sh 'python build.py build --platform mac --build-system Xcode --config Release --module testboden'
            }
        }

        /* tests for ios should be called with this commandline:
        sh 'mkdir -p testresults'
        sh 'python build.py --platform ios --build-system --config Debug --module testboden --run-output-file testresults/ios_testboden.xml -- run --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
        junit "testresults/ios_testboden.xml"
        */


    }
}
